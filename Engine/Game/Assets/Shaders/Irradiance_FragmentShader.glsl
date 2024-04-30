#version 460 core

in vec3 texcoords;
out vec4 color;
uniform samplerCube environment;


void main()
{
	vec3 normal = normalize(texcoords);


	//color = texture(cube, texcoords);
	color = vec4(1.0f,0.0f,0.0f,1.0f);
}