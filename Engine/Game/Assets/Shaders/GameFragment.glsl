#version 460 core

uniform sampler2D screenTex;
in vec2 uv;
out vec4 col;

void main()
{
	col = texture(screenTex, uv);
}