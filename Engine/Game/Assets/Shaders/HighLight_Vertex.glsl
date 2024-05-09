#version 460 core

#extension GL_ARB_bindless_texture : require

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNorm;
layout(location = 3) in vec4 inTang;

layout(std140, binding = 0) uniform CameraMatrices{
	mat4 view;
	mat4 proj;
};

readonly layout(std430, row_major, binding = 10) buffer Transforms {
	mat4 models[];
};

void main()
{
	mat4 model = models[gl_BaseInstance];

	float outline = 0.15;

	//float scale = 1.03;
	//model[0][0] *= scale;
	//model[0][1] *= scale;
	//model[0][2] *= scale;
	//model[0][3] *= scale;
	//
	//model[1][0] *= scale;
	//model[1][1] *= scale;
	//model[1][2] *= scale;
	//model[1][3] *= scale;
	//
	//model[2][0] *= scale;
	//model[2][1] *= scale;
	//model[2][2] *= scale;
	//model[2][3] *= scale;

	vec3 crntPos = vec3(model * vec4(inPos + inNorm * outline, 1.0));

	gl_Position = proj * view * vec4(crntPos, 1);
}