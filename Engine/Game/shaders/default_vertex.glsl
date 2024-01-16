#version 440
layout(location = 0) in vec3 my_vertex_position;
layout (location = 1) in vec3 normal;
// layout(location = 2) in vec2 vertex_uv0;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 viewProj;


out vec4 color;

void main()
{
	gl_Position = viewProj * model * vec4(my_vertex_position, 1.0);
	color = vec4(abs(normal), 1.0);
}