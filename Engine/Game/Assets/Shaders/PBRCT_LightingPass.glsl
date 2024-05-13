#version 460 core
#define PI 3.1415926535897932384626433832795

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
	float radius;
	vec4 pos; //w intensity
	vec4 aimD;//w cos inner angle
	vec4 col;//w cos outer angle
};
readonly layout(std430, binding = 1) buffer SpotLights
{
	uint numSLights;
	SpotLight sLights[];
};

in vec2 uv;

layout(location = 1)uniform vec3 cPos;
//Gbuffer
uniform sampler2D diffuseTex;
uniform sampler2D specularRoughTex;
uniform sampler2D normalTex;
uniform sampler2D positionTex;
uniform sampler2D emissiveTex;
//Ambient
uniform samplerCube prefilteredIBL;
uniform samplerCube diffuseIBL;
uniform sampler2D environmentBRDF;
uniform uint numLevels;

vec3 cDiff;
vec3 cSpec;
float rough;
vec3 N;
vec3 pos;
vec3 V;
float depth;
vec3 emissiveCol;

layout(location = 5) out vec4 outColor;

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
	cDiff = texture(diffuseTex, uv).rgb;
	vec4 specColorTex = texture(specularRoughTex, uv);
	cSpec = specColorTex.rgb;
	rough = specColorTex.rgb.a;
	N = normalize(texture(normalTex, uv).rgb * 2.0 - 1.0);
	vec4 posDepth = texture(diffuseTex, uv);
	pos = posDepth.rgb;
	depth = posDepth.a;
	emissiveCol = texture(emissiveTex, uv).rgb;
	V = normalize(cPos - pos);

	vec3 pbrCol = vec3(0);
	pbrCol += GetPBRLightColor(dirDir.xyz, dirCol.xyz, dirCol.w, 1);
	
	//Point lights
	for(int i = 0; i<numPLights; ++i)
	{
		vec3 mVector = pos - pLights[i].pos.xyz;
		float dist = length(mVector);
		vec3 pDir = normalize(mVector);
		float att = pow(max(1 - pow(dist/pLights[i].pos.w,4), 0.0),2) / (dist*dist + 1);
		pbrCol += GetPBRLightColor(pDir, pLights[i].col.rgb,  pLights[i].col.w, att);
	}
	
	//Spot lights
	for(int i = 0; i<numSLights; ++i)
	{
		vec3 mVector = pos - sLights[i].pos.xyz;
		vec3 sDir = normalize(mVector);
		vec3 aimDir = normalize(sLights[i].aimD.xyz);
		float dist = dot(mVector, aimDir);
		//TODO: Check that the radius of spot light is correct
		float r = sLights[i].radius;
		float att = pow(max(1 - pow(dist/r,4), 0),2) / (dist*dist + 1);
		float c = dot(sDir, aimDir);
		float cInner = sLights[i].aimD.w;
		float cOuter = sLights[i].col.w;
		//float cAtt = 1;
		//if(cInner > c && c > cOuter)
			//cAtt = (c - cOuter) / (cInner - cOuter);
		float cAtt = clamp((c - cOuter) / (cInner - cOuter), 0.0, 1.0);
		att *= cAtt;
		pbrCol += GetPBRLightColor(sDir, sLights[i].col.rgb,  sLights[i].pos.w, att);
	}

	pbrCol += GetAmbientLight();
	pbrCol += emissiveCol;

	//HDR color  
	vec3 hdrCol = pbrCol;
	
	//LDR color with reinhard tone Mapping
	vec3 ldrCol = hdrCol / (hdrCol.rgb + vec3(1.0));

	//Gamma correction
	ldrCol = pow(ldrCol, vec3(1/2.2));
	
	//Output
	outColor = vec4(ldrCol, 1.0f);
}