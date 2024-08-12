#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNorm;
layout(location = 3) in vec4 inTang;


layout(std140, binding = 0) uniform CameraMatrices
{
	mat4 view;
	mat4 proj;
	mat4 invView;
};

readonly layout(std430, row_major, binding = 10) buffer Transforms 
{
	mat4 models[];
};

void main()
{
	uint instace_index =  gl_BaseInstance;
	mat4 model = models[gl_BaseInstance];

	gl_Position = proj * view * model * vec4(inPos,1);
}