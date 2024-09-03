#version 460 core

layout(location = 0) in vec3 inPos;

out vec4 clipping;

layout(location = 1) uniform mat4 model;

layout(std140, binding = 0) uniform CameraMatrices{
	mat4 view;
	mat4 proj;
	mat4 invView;
};


void main()
{
	clipping = proj * view * model * vec4(inPos,1);
	gl_Position = clipping;
}