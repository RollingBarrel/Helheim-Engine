#version 460 core

in vec3 texcoords;
out vec4 color;
uniform sampler2D HDRImage;


#define PI 3.1415926535897932384626433832795

vec2 CartesianToEquirectangular(in vec3 dir)
{

	float phi;
	phi = atan(dir.z, dir.x);
	phi = phi/(2.0*PI) + 0.5;
	
	float theta = asin(dir.y);
	theta = (theta/PI+0.5);

	return vec2(phi, 1.0-theta);
}

void main()
{
	vec3 dir = normalize(texcoords);
	vec2 uv = CartesianToEquirectangular(dir);
	color = texture(HDRImage,uv);
}