#version 460 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNorm;
layout(location = 3) in vec4 inTang;


layout(location = 0) uniform mat4 model;
layout (location = 1)uniform vec3 lDir;
layout (location = 2)uniform vec3 cPos;

layout(std140, binding = 0) uniform CameraMatrices{
	mat4 view;
	mat4 proj;
};


out VertToFrag {
	vec2 uv;
	flat vec3 tLightDir;
	vec3 tPos;
	flat vec3 tCameraPos;
	vec3 tNorm;
};

void main()
{
	vec3 N = normalize(vec3(model * vec4(inNorm, 0.0)));
	vec3 T = normalize(vec3(model * vec4(inTang.xyz, 0.0))); 
	vec3 B = inTang.w * cross(N, T);
	mat3 TBNInv = transpose(mat3(T,B,N));

	uv = inUv;
	tLightDir = TBNInv * lDir;
	vec4 modelPos = model * vec4(inPos,1);
	tPos = TBNInv * modelPos.xyz;
	//Calculate the normal in tangent space because we need it if the model does not have a normal map
	tNorm = TBNInv * (model * vec4(inNorm,1)).xyz;
	tCameraPos = TBNInv * cPos;
	gl_Position = proj * view * modelPos;
}