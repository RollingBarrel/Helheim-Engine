#version 460 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 proj;

out vec2 uv0;

void main()
{
	uv0 = uv;
	gl_Position = proj * view * model * vec4(pos, 0.0, 1.0);
	//gl_Position = vec4(pos, 0.0, 1.0);
}
