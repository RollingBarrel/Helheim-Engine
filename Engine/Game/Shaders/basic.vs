#version 430
layout(location=0) in vec3 my_vertex_position;

uniform mat4 viewproj;
uniform mat4 model;


void main()
{
 gl_Position = viewproj*model*vec4(my_vertex_position, 1.0);
}