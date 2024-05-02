#version 460 core

in vec3 texcoords;
out vec4 fragColor;
uniform samplerCube environment;

layout(location = 3) uniform float roughness;

#define NUM_SAMPLES 4096
#define PI 3.1415926535897932384626433832795

vec3 GetHemisphereSample(float u1, float u2)
{
	float phi = u1 * 2.0 * PI;
	float r = sqrt(u2);
	return vec3(r * cos(phi), r * sin(phi), sqrt(1 - u2));
}

mat3 ComputeTangentSpace(vec3 normal)
{
	vec3 up = abs(normal.y) > 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up = cross(normal, right);
	return mat3(right, up, normal);
}

float RadicalInverseVdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley2D(uint i, uint N)
{
	return vec2(float(i) / float(N), RadicalInverseVdC(i));
}

vec3 HemisphereSampleGGX(float u1, float u2, float roughness)
{
	float a = roughness * roughness;
	float phi = 2.0 * PI * u1;
	float cos_theta = sqrt((1.0 - u2) / (u2 * (a * a - 1) + 1));
	float sin_theta = sqrt(1 - cos_theta * cos_theta);
	// spherical to cartesian conversion
	vec3 dir;
	dir.x = cos(phi) * sin_theta;
	dir.y = sin(phi) * sin_theta;
	dir.z = cos_theta;
	return dir;
}

void main()
{
	vec3 R = normalize(texcoords);
	vec3 N = R, V = R;
	vec3 color = vec3(0.0);
	float weight = 0.0f;
	mat3 tangentSpace = ComputeTangentSpace(N);
	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		vec2 rand_value = Hammersley2D(i, NUM_SAMPLES);
		vec3 H = tangentSpace * HemisphereSampleGGX(rand_value[0], rand_value[1], roughness);
		vec3 L = reflect(-V, H);
		float NdotL = max(dot(N, L), 0.0);
		if (NdotL > 0)
		{
			color += texture(environment, L).rgb * NdotL;
			weight += NdotL;
		}
	}
	fragColor = vec4(color / weight, 1.0);
}