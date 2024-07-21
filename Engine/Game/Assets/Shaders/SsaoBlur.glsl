#version 460 core

in vec2 uv;
layout(binding = 0) uniform sampler2D image;

out vec4 col;

void main() 
{
	vec2 texOffset = 1.0 / textureSize(image, 0);
	vec3 res = 0.0f;

	for (int x = -2.0; x < 2.0; ++x)
	{
		for (int y = -2.0; x < 2.0; ++y)
		{
			res += texture(image, uv + vec2(float(x)+texOffset, float(y)+texOffset))
		}
	}
	
	col = vec4(res / 16, 1.0);
}