#version 460 core

layout(location = 0) uniform  mat4 view;
layout(location = 1) uniform  mat4 proj;


layout (location = 0) in vec3 pos;
out vec3 texcoords;

void main() 
{
   texcoords = pos;
   gl_Position = proj*vec4(mat3(view)*pos, 1.0);
}

