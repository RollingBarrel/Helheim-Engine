#version 460 core

in vec2 uv;
layout(binding = 0) uniform sampler2D image;
layout(location = 0) uniform int halfKernel;

out vec4 col;

void main() 
{
	vec2 texOffset = 1.0f / textureSize(image, 0);
	vec3 res = vec3(0.0f);

	for (int x = -halfKernel; x < halfKernel; ++x)
	{
		for (int y = -halfKernel; y < halfKernel; ++y)
		{
			res += texture(image, uv + vec2(float(x) * texOffset.x, float(y) * texOffset.y)).rgb;
		}
	}
	
	col = vec4(res / ((halfKernel + halfKernel)*(halfKernel + halfKernel)), 1.0f);
}