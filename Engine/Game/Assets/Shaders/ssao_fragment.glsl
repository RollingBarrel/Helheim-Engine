#version 460 core

out float color;
in vec2 uv;
//uniform sampler2D positions;
//uniform sampler2D normals;
//uniform vec3 kernel_samples[24]

void main()
{
	
	color = 0.0f + gl_FragCoord.x / 500 ;
}