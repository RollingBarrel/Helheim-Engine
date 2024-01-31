#version 460 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNorm;
layout(location = 3) in vec4 inTang;


layout(location = 0) uniform mat4 model;

layout(std140, binding = 0) uniform CameraMatrices{
	mat4 view;
	mat4 proj;
};


out VertToFrag {
	vec2 uv;
	vec3 sPos;
	vec3 norm;
	vec4 tang;
};

void main()
{
	uv = inUv;
	//transpos inverse on position and normal:)
	sPos = transpose(inverse(mat3(model)))*inPos;
	norm = transpose(inverse(mat3(model)))*inNorm;
	tang = inTang;
	gl_Position = proj * view * model * vec4(inPos,1);
}