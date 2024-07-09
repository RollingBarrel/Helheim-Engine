#version 460 core

#define TANGENT_ROWS 10
#define TANGENT_COLS 10
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



layout(location=1) uniform vec3 kernel_samples[KERNEL_SIZE];
layout(location=5) uniform uvec2 screenSize;
layout(location=0) uniform vec3 randomTangents [TANGENT_ROWS][TANGENT_COLS];


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

void main()
{
	vec3 position = (view* vec4(texture(positions, uv).xyz,1.0)).xyz;
	vec3 normal = mat3(view)*normalize(texture(normals, uv).xyz);
	mat3 tangent_space = createTangentSpace(normal, getRandomTangent());
}