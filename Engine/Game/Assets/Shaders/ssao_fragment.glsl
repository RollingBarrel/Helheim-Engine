#version 460 core

#define TANGENT_ROWS 2
#define TANGENT_COLS 2
#define KERNEL_SIZE 24
out vec4 ambientOcclusion;
out vec3 color;
in vec2 uv;

layout(std140, binding = 0) uniform CameraMatrices
{
	mat4 view;
	mat4 proj;
};


layout(binding = 0) uniform sampler2D positions;
layout(binding = 1) uniform sampler2D normals;


layout(location=0) uniform vec3 randomTangents [2][2];
uniform vec3 kernelSamples[24];
uniform uvec2 screenSize;



mat3 createTangentSpace(const vec3 normal, const vec3 up)
{
	vec3 tangent = normalize(up-normal*dot(normal, up)); // Gram-Schmidt
	vec3 bitangent = normalize(cross(tangent, normal));
	return mat3(bitangent, tangent, normal);
}

vec3 getRandomTangent()
{
	vec2 screenPos = uv*vec2(screenSize);
	ivec2 index = ivec2(int(mod(screenPos.y, TANGENT_ROWS)), int(mod(screenPos.x, TANGENT_COLS)));
	return randomTangents[index.x][index.y];
}

float getSceneDepthAtSamplePos(in vec3 samplePos)
{
	vec4 clippingSpace = proj*vec4(samplePos, 1.0);
	vec2 sampleUV = (clippingSpace.xy/clippingSpace.w)*0.5+0.5;
	return (view*texture(positions, sampleUV)).z;
}

void main()
{
//RECUERDA DE CAMBIAR EL OPENGL A SOLO UN COLOR
	vec3 position = (view* vec4(texture(positions, uv).xyz,1.0)).xyz;
	vec3 normal = mat3(view)*normalize(texture(normals, uv).xyz);
	mat3 tangentSpace = createTangentSpace(normal, getRandomTangent());
	int occlusion = 0;
	float range = 2.0;
	float bias = 1.0;
	for(int i=0; i< KERNEL_SIZE; ++i)
	{
		vec3 samplePos = position+tangentSpace*kernelSamples[i];
		float samplePosDepth= getSceneDepthAtSamplePos(samplePos);
		if(getSceneDepthAtSamplePos(samplePos) > samplePos.z) ++occlusion;
		//if(samplePosDepth + bias > samplePos.z &&  abs(samplePosDepth-position.z) < range)
		//{
		//	//float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - samplePosDepth));
		//	//occlusion += (samplePosDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
		//	++occlusion;
		//}			

	}
	//ambientOcclusion = vec4(vec3(1.0-float(occlusion)/float(KERNEL_SIZE)), 1.0f);
	//vec3 occlusionFactor = vec3(texture(ambientOcclusion,uv).r);
	color= vec3(float(occlusion)/float(KERNEL_SIZE));
}