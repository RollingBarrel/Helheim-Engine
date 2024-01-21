#version 440
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 proj;
layout (location = 0) in vec3 pos;
out vec3 texcoords;

void main() 
{
   texcoords = pos;
   vec4 p = proj*vec4(mat3(view)*pos, 1.0);
   gl_Position = p;
}

