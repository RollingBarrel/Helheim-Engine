#version 460 core
layout(std140, binding = 0) uniform CameraMatrices{
	mat4 view;
	mat4 proj;
};
layout (location = 0) in vec3 pos;
out vec3 texcoords;

void main() 
{
   texcoords = pos;
   gl_Position = proj*vec4(mat3(view)*pos, 1.0);
}

