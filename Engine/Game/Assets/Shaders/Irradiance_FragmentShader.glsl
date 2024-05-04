#version 460 core

#define NUM_SAMPLES 2048
#define PI 3.1415926535897932384626433832795

in vec3 texcoords;
out vec4 fragColor;
uniform samplerCube environment;

layout(location = 5) uniform uint environmentFaceWidth;

float computeLod(float pdf, int numSamples, uint width)
{
	// note that 0.5 * log2 is equivalent to log4
	return max(0.5 * log2(6.0 * float(width) * float(width) / (float(numSamples) * pdf)), 0.0);
}

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

void main()
{
	vec3 irradiance = vec3(0.0);
	vec3 normal = normalize(texcoords);
	mat3 tangentSpace = ComputeTangentSpace(normal);

	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		vec2 randValue = Hammersley2D(i, NUM_SAMPLES);
		//vec3 L = tangentSpace * GetHemisphereSample(randValue[0], randValue[1]);
		//vec3 Li = texture(environment, L).rgb;
		vec3 L = GetHemisphereSample(randValue[0], randValue[1]);
		float cosTheta = L.z;
		float pdf = cosTheta / PI;
		float lod = computeLod(pdf, NUM_SAMPLES, environmentFaceWidth);
		L = tangentSpace * L;
		vec3 Li = textureLod(environment, L, lod).rgb;
		irradiance += Li;
	}

	fragColor = vec4(irradiance*(1.0/float(NUM_SAMPLES)), 1.0f);
}