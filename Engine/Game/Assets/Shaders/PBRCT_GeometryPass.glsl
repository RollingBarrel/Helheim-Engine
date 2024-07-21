#version 460 core
#extension GL_ARB_bindless_texture : require

struct Material
{
	vec3 baseColor;
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


in VertToFrag {
	vec2 uv;
	vec3 sPos;
	vec3 norm;
	vec4 tang;
	flat uint instace_index;
};

layout(location = 0) out vec3 outDiffuse;
//layout(location = 1) out vec4 outSpecularRough;
layout(location = 1) out vec3 outMetalRough;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outPosition;
layout(location = 4) out vec3 outEmissive;


void main() 
{
	Material material = materials[indices[instace_index].matIdx];

	vec3 baseColor = material.baseColor;
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
		N = normalize(texture(material.normalTex, uv).rgb * 2.0 - 1.0);
		N = normalize(TBN * N);
	}
	else
	{
		N = normalize(norm);  	//Normal
	}
	if (material.hasEmissiveTex)
	{
		vec3 emissiveColor = vec3(texture(material.emissiveTex, uv));
		//Using  gamma correction forces to transform sRGB textures to linear space
		emissiveColor = pow(emissiveColor, vec3(2.2));
		emissiveColor *= material.emissiveFactor;
		outEmissive = emissiveColor;
	}

	outDiffuse = baseColor;
	outMetalRough.b = metal;
	outMetalRough.g = rough;


	//outDiffuse = baseColor * (1 - metal);
	//outSpecularRough.rgb = mix(vec3(0.04), baseColor, metal);
	//outSpecularRough.a = rough;
	outNormal = N * 0.5 + 0.5;
	outPosition = sPos;
	
	//outDepth = gl_FragCoord.z;

	
}