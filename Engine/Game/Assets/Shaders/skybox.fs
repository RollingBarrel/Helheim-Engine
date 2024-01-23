#version 460 core
in vec3 texcoords;
out vec4 color;
uniform samplerCube cube;
void main()
{
 color = texture(cube, texcoords);
 }