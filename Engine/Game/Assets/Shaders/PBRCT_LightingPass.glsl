#version 460 core
#define PI 3.1415926535897932384626433832795
#extension GL_ARB_bindless_texture : require

layout(std140, binding = 0) uniform CameraMatrices
{
	mat4 view;
	mat4 proj;
	mat4 invView;
};

//Light properties
layout(std140, binding = 1) uniform DirLight
{
	vec3 dirDir;
	vec4 dirCol; //w is the intensity (0-5)
};
struct PointLight
{
	vec4 pos; //w is the radius
	vec4 col;//a is intensity
};
readonly layout(std430, binding = 0) buffer PointLights
{
	uint numPLights;
	PointLight pLights[];
};
struct SpotLight
{
	vec4 pos; //w intensity
	vec4 aimD;//w cos inner angle
	vec4 col; //w cos outer angle
	float radius;
	int shadowIndex;
};
readonly layout(std430, binding = 1) buffer SpotLights
{
	uint numSLights;
	SpotLight sLights[];
};


struct Shadow
{
	mat4 viewProjMatrix;
	sampler2D shadowMap;
	float bias;
};

readonly layout(std430, binding = 4) buffer SpotLightShadows
{
	Shadow shadows[];
};

in vec2 uv;

//Gbuffer
layout(binding = 0)uniform sampler2D diffuseTex;
//layout(binding = 1)uniform sampler2D specularRoughTex;
layout(binding = 1)uniform sampler2D metalRoughTex;
layout(binding = 2)uniform sampler2D normalTex;
layout(binding = 3)uniform sampler2D positionTex;
layout(binding = 4)uniform sampler2D emissiveTex;
//Ambient
layout(binding = 5)uniform samplerCube prefilteredIBL;
layout(binding = 6)uniform samplerCube diffuseIBL;
layout(binding = 7)uniform sampler2D environmentBRDF;
uniform uint numLevels;

layout(binding = 10) uniform isamplerBuffer pointLightList;
layout(binding = 11) uniform isamplerBuffer spotLightList;
layout(location = 2) uniform uint lightListSize;
layout(location = 3) uniform uvec2 numTiles;
layout(location = 4) uniform uvec2 tileSize;

layout(binding = 9) uniform sampler2D ambientOcclusion;
uniform bool activeAO;

vec3 cDif;
vec3 cSpec;
float rough;
vec3 N;
float depth;
vec3 pos;
vec3 V;
vec3 emissiveCol;

out vec4 outColor;

vec3 GetPBRLightColor(vec3 lDir, vec3 lCol, float lInt, float lAtt)
{
	vec3 L =  -normalize(lDir); 	//Light direction
	vec3 H = normalize(L+V);
	vec3 Li = lInt * lAtt * lCol.rgb;  //Incoming radiance
	float dotNL = max(dot(N,L),0.001f);
	float dotLH = max(dot(L,H),0.001f);
	float dotNH = max(dot(N,H),0.001f);
	float dotNV = max(dot(N,V),0.001f);

	vec3 fresnel = cSpec + (1-cSpec) * pow(1-dotLH,5);
	float smithVisibility = 0.5 / (dotNL*(dotNV*(1-rough)+rough)+dotNV*(dotNL*(1-rough)+rough));
	float ggxDenominator = dotNH*dotNH*(rough*rough-1) + 1;
	float ggx = (rough*rough) / (PI*ggxDenominator*ggxDenominator);
	vec3 pbrColor = ( cDif*(1-cSpec) + 0.25 * fresnel * smithVisibility * ggx ) * Li * dotNL;
	return pbrColor;
}

vec3 GetAmbientLight()
{
	float dotNV = clamp(dot(N, V), 0.001, 1.0);
	vec3 irradiance = texture(diffuseIBL, N).rgb;
	vec3 R = reflect(-V, N);
	vec3 radiance = textureLod(prefilteredIBL, R, rough * numLevels).rgb;
	vec2 fab = texture(environmentBRDF, vec2(dotNV, rough)).rg;
	vec3 diffuse = (cDif * (1 - cSpec));
	return diffuse * irradiance + radiance * (cSpec * fab.x + fab.y);
}

void main() 
{
	//cDif = texture(diffuseTex, uv).rgb;
	//vec4 specColorTex = texture(specularRoughTex, uv);
	//cSpec = specColorTex.rgb;
	//rough = max(specColorTex.a * specColorTex.a, 0.001f);
	N = normalize(texture(normalTex, uv).rgb * 2.0 - 1.0);
	//depth = texture(depthTex, uv).r;
	//pos = GetWorldPos(depth, uv);
	pos = texture(positionTex, uv).rgb;
	emissiveCol = texture(emissiveTex, uv).rgb;
	vec3 cameraPos = vec3(invView[3][0], invView[3][1], invView[3][2]);
	V = normalize(cameraPos - pos);

	vec3 baseColor = texture(diffuseTex, uv).rgb;
	vec2 specColorTex = texture(metalRoughTex, uv).gb;
	float metal = specColorTex.y;
	rough = max(specColorTex.x * specColorTex.x, 0.001f);
	
	cDif = baseColor * (1 - metal);
	cSpec = mix(vec3(0.04), baseColor, metal);


	vec3 pbrCol = vec3(0);
	//Directional light
	pbrCol += GetPBRLightColor(dirDir.xyz, dirCol.xyz, dirCol.w, 1);
	
	const uvec2 currTile = uvec2(gl_FragCoord.xy) / tileSize;
	const uint tileIdx = currTile.y * numTiles.x + currTile.x;
	//Point lights
	int idx = texelFetch(pointLightList, int(tileIdx * lightListSize)).x;
	for(uint i = 0; i < lightListSize && idx != -1; idx = (texelFetch(pointLightList, int(tileIdx * lightListSize + i))).x)
	{
		PointLight pLight = pLights[idx];
		vec3 mVector = pos - pLight.pos.xyz;
		float dist = length(mVector);
		vec3 pDir = normalize(mVector);
		float att = pow(max(1 - pow(dist/ pLight.pos.w,4), 0.0),2) / (dist*dist + 1);
		pbrCol += GetPBRLightColor(pDir, pLight.col.rgb, pLight.col.w, att);
		++i;
	}
	
	//Spot lights
	idx = texelFetch(spotLightList, int(tileIdx * lightListSize)).x;
	for (int i = 0; i < lightListSize && idx != -1; idx = (texelFetch(spotLightList, int(tileIdx * lightListSize + i))).x)
	{
		SpotLight sLight = sLights[idx];
		//Shadows
		float shadowValue = 1.0;
		if (sLight.shadowIndex >= 0)
		{
			vec4 lightClipSpace = shadows[sLight.shadowIndex].viewProjMatrix * vec4(pos, 1);
			vec3 lightNDC = lightClipSpace.xyz / lightClipSpace.w;
			lightNDC.xyz = lightNDC.xyz * 0.5 + 0.5;
			float shadowDepth = texture(shadows[sLight.shadowIndex].shadowMap, lightNDC.xy).r + shadows[sLight.shadowIndex].bias;
			float fragmentDepth = lightNDC.z;

			if(!(lightNDC.x >= 0.0 && lightNDC.x <= 1.0f &&
				lightNDC.y >= 0.0 && lightNDC.y <= 1.0f &&
				fragmentDepth < shadowDepth))
				{
					shadowValue = 0.0;
				}
		}

		vec3 mVector = pos - sLight.pos.xyz;
		vec3 sDir = normalize(mVector);
		vec3 aimDir = normalize(sLight.aimD.xyz);
		float dist = dot(mVector, aimDir);
		//TODO: Check that the radius of spot light is correct
		float r = sLight.radius;
		float att = pow(max(1 - pow(dist / r, 4), 0), 2) / (dist * dist + 1);
		float c = dot(sDir, aimDir);
		float cInner = sLight.aimD.w;
		float cOuter = sLight.col.w;
		//float cAtt = 1;
		//if(cInner > c && c > cOuter)
			//cAtt = (c - cOuter) / (cInner - cOuter);
		float cAtt = clamp((c - cOuter) / (cInner - cOuter), 0.0, 1.0);
		att *= cAtt;
		pbrCol += GetPBRLightColor(sDir, sLight.col.rgb, sLight.pos.w, att) * shadowValue;
		++i;
	}

	vec3 occlusionFactor = vec3(1.0);
	if (activeAO)
	{
		occlusionFactor = vec3(texture(ambientOcclusion, uv).r);
	}
	pbrCol += GetAmbientLight() * occlusionFactor;
	pbrCol += emissiveCol;

	//HDR color  
	vec3 hdrCol = pbrCol;

	//Output
	outColor = vec4(hdrCol, 1.0f);
}