#version 460 core

layout(location = 0) in vec3 inPos;

out vec3 sPos

layout(std140, binding = 0) uniform CameraMatrices{
	mat4 model;
	mat4 view;
	mat4 proj;
};


void main()
{
	sPos = (model*vec4(inPos,1)).xyz;
	gl_Position = proj * view * model * vec4(inPos,1);
}