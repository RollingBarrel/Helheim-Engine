#version 460 core

in vec2 uv;
out vec4 color;

layout(std140, binding = 0) uniform CameraMatrices{
	mat4 view;
	mat4 proj;
};
layout(binding = 0) uniform sampler2D depthTex;
layout(location = 0) uniform mat4 invView;
layout(location = 1) uniform vec3 fogColor;
layout(location = 2) uniform float maxFog;
layout(location = 3) uniform float density;
layout(location = 4) uniform float heightFallof;

float GetLinearZ(float inputDepth)
{
	return -proj[3][2] / (proj[2][2] + (inputDepth * 2.0 - 1.0));
}

vec3 GetWorldPos(float depth, vec2 uvs)
{
	float viewZ = GetLinearZ(depth);
	float viewX = (-viewZ * (uvs.x * 2.0 - 1.0)) / proj[0][0];
	float viewY = (-viewZ * (uvs.y * 2.0 - 1.0)) / proj[1][1];
	return (invView * vec4(vec3(viewX, viewY, viewZ), 1.0)).xyz;
}

//t = distance to point  // ro = cameraPos  // rd = camera-position vector
float ApplyFog(in float t, in vec3 ro, in vec3 rd)
{
	return (density / heightFallof) * exp(-ro.y * heightFallof) * (1.0 - exp(-t * rd.y * heightFallof)) / rd.y;
}

void main()
{
	vec3 worldPos = GetWorldPos(texture(depthTex, uv).r, uv);
	vec3 cameraPos = vec3(invView[3][0], invView[3][1], invView[3][2]);
	vec3 rayDir = worldPos - cameraPos;
	float distance = length(rayDir);

	float fogAmount = min(ApplyFog(distance, cameraPos, rayDir / distance), maxFog);
	color = vec4(fogColor, fogAmount);
}