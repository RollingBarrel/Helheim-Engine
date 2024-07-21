#version 460 core

in vec2 uv;
layout(location = 0) uniform bool horizontal;
layout(binding = 0) uniform sampler2D image;

uniform float gaussianWeights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
out vec4 col;

void main() 
{
	vec2 texOffset = 1.0 / textureSize(image, 0);
	vec3 res = texture(image, uv).rgb * gaussianWeights[0];
	if (horizontal)
	{
		for (unsigned int i = 1; i < 5; ++i)
		{
			res += texture(image, uv + vec2(texOffset.x * i, 0.0)).rgb * gaussianWeights[i];
			res += texture(image, uv - vec2(texOffset.x * i, 0.0)).rgb * gaussianWeights[i];
		}
	}
	else
	{
		for (unsigned int i = 1; i < 5; ++i)
		{
			res += texture(image, uv + vec2(0.0, texOffset.y * i)).rgb * gaussianWeights[i];
			res += texture(image, uv - vec2(0.0, texOffset.y * i)).rgb * gaussianWeights[i];
		}
	}
	col = vec4(res, 1.0);
}