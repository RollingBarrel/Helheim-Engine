#version 460 core

#define TANGENT_ROWS 20
#define TANGENT_COLS 20
#define KERNEL_SIZE 24

out float color;
in vec2 uv;

layout(std140, binding = 0) uniform CameraMatrices
{
	mat4 view;
	mat4 proj;
};


layout(binding = 0) uniform sampler2D positions;
layout(binding = 1) uniform sampler2D normals;


uniform vec3 randomTangents [TANGENT_ROWS * TANGENT_COLS];
uniform vec3 kernelSamples[KERNEL_SIZE];
uniform uvec2 screenSize;
layout(location = 1) uniform float range;
layout(location = 2) uniform float bias;


mat3 CreateTangentSpace(const vec3 normal, const vec3 up)
{
	vec3 tangent = normalize(up-normal*dot(normal, up)); // Gram-Schmidt
	vec3 bitangent = normalize(cross(tangent, normal));
	return mat3(bitangent, tangent, normal);
}

vec3 GetRandomTangent()
{
	vec2 screenPos = uv*vec2(screenSize);
	ivec2 index = ivec2(int(mod(screenPos.y, TANGENT_ROWS)), int(mod(screenPos.x, TANGENT_COLS)));
	return randomTangents[index.y * TANGENT_ROWS + index.x];
}

float GetSceneDepthAtSamplePos(in vec3 samplePos)
{
	vec4 clippingSpace = proj*vec4(samplePos, 1.0);
	vec2 sampleUV = (clippingSpace.xy/clippingSpace.w)*0.5+0.5;
	return (view * vec4(texture(positions, sampleUV).xyz, 1.0)).z;
}

void main()
{
//RECUERDA DE CAMBIAR EL OPENGL A SOLO UN COLOR
	vec3 position = (view * vec4(texture(positions, uv).xyz,1.0)).xyz;
	vec3 normal = mat3(view)*normalize(texture(normals, uv).xyz * 2.0 - 1.0);
	mat3 tangentSpace = CreateTangentSpace(normal, GetRandomTangent());
	float occlusion = 0;

	for(int i=0; i< KERNEL_SIZE; ++i)
	{
		vec3 samplePos = position+tangentSpace*kernelSamples[i];
		float samplePosDepth= GetSceneDepthAtSamplePos(samplePos);
		if(samplePosDepth + bias > samplePos.z &&  abs(samplePosDepth-position.z) < range)
		{
			float rangeCheck = smoothstep(0.0, 1.0, 1 / abs(position.z - samplePosDepth));
			occlusion += (samplePosDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
		}			
	}

	//vec3 occlusionFactor = vec3(texture(ambientOcclusion,uv).r);
	color = 1.0 - occlusion / float(KERNEL_SIZE);
}