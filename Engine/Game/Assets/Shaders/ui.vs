#version 460 core

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 vertex_uv0;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 proj;

out vec2 uv0;

void main()
{
	gl_Position = proj * view * model * vec4(vertex_position, 1.0, 1.0);
	uv0 = vertex_uv0;
}
