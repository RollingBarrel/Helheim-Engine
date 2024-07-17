#version 460 core

in vec2 uv;
out vec4 col;

layout(binding = 0) uniform sampler2D inputTex;

void main() 
{
	vec2 texSize = vec2(textureSize(inputTex, 0));
	vec2 halfTexel = 0.5 / texSize;
	vec4 sum = texture(inputTex, uv) * 4.0;
	sum += texture(inputTex, uv + vec2(-halfTexel.x, halfTexel.y));
	sum += texture(inputTex, uv + vec2(halfTexel.x, halfTexel.y));
	sum += texture(inputTex, uv + vec2(-halfTexel.x, -halfTexel.y));
	sum += texture(inputTex, uv + vec2(halfTexel.x, -halfTexel.y));
	col = sum / 8.0;
}