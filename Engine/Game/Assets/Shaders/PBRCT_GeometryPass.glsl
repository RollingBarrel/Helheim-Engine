#version 460 core
#extension GL_ARB_bindless_texture : require

struct Material
{
	vec4 baseColor;
	sampler2D baseColorTex;
	sampler2D metalRoughTex;
	sampler2D normalTex;
	bool hasMetalRoughTex;
	bool hasBaseColorTex;
	bool hasNormalMap;
	float metal;
	float rough;
	bool hasEmissiveTex;
	vec3 emissiveFactor;
	sampler2D emissiveTex;
};
readonly layout(std430, binding = 11) buffer Materials 
{
	Material materials[];
};

struct Index
{
	uint matIdx;
};
readonly layout(std430, binding = 12) buffer Indices 
{
	Index indices[];
};

layout(binding = 0) uniform sampler2D noiseTex;


in VertToFrag {
	vec2 uv;
	vec3 sPos;
	vec3 norm;
	vec4 tang;
	flat uint instace_index;
};

layout(location = 0) out vec3 outDiffuse;
//layout(location = 1) out vec4 outSpecularRough;
layout(location = 1) out vec2 outMetalRough;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outPosition;
layout(location = 4) out vec3 outEmissive;


void main() 
{
	Material material = materials[indices[instace_index].matIdx];

	float noiseValue = min(texture(noiseTex, uv).x, 1.0f);
	if (material.baseColor.a <= noiseValue) 
		discard;

	vec3 baseColor = material.baseColor.rgb;
	if(material.hasBaseColorTex)
	{
		vec3 texCol = vec3(texture(material.baseColorTex, uv));
		//Using  gamma correction forces to transform sRGB textures to linear space
		texCol = pow(texCol, vec3(2.2));
		baseColor *= texCol;
	}
	float metal = material.metal;
	float rough = material.rough;
	if(material.hasMetalRoughTex)
	{
		vec2 metRough = texture(material.metalRoughTex, uv).gb;
		rough *= metRough.x;
		metal *= metRough.y;
	}
	vec3 N = vec3(0.0f);
	if (material.hasNormalMap)
	{
		N = normalize(norm);
		vec3 T = normalize(tang.xyz); 
		vec3 B = normalize(tang.w * cross(N, T));
		mat3 TBN = mat3(T,B,N);
		N = vec3(texture(material.normalTex, uv).rg * 2.0f - 1.0f, 1.0f);
		N.z = sqrt(1 - N.x * N.x - N.y * N.y);
		N = normalize(N);
		N = normalize(TBN * N);
	}
	else
	{
		N = normalize(norm);  	//Normal
	}
	vec3 emissiveColor = vec3(0.0f);
	if (material.hasEmissiveTex)
	{
		emissiveColor = vec3(texture(material.emissiveTex, uv));
		//Using  gamma correction forces to transform sRGB textures to linear space
		emissiveColor = pow(emissiveColor, vec3(2.2));
		emissiveColor *= material.emissiveFactor;
		outEmissive = emissiveColor;
	}

	if (material.baseColor.a < (noiseValue + 0.09))
		outEmissive = vec3(1.0f, 0.0f, 0.0f);

	outDiffuse = baseColor;
	outMetalRough.r = rough;
	outMetalRough.g = metal;


	//outDiffuse = baseColor * (1 - metal);
	//outSpecularRough.rgb = mix(vec3(0.04), baseColor, metal);
	//outSpecularRough.a = rough;
	outNormal = N * 0.5 + 0.5;
	outPosition = sPos;
	
	//outDepth = gl_FragCoord.z;

	
}