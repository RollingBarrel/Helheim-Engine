#version 460 core

layout(location = 0) out vec3 outDiffuse;
layout(location = 1) out vec4 outSpecularRough;
//layout(location = 2) out vec3 outNormal;
//layout(location = 4) out vec3 outEmissive;

layout(std140, binding = 0) uniform CameraMatrices{
	mat4 view;
	mat4 proj;
};



layout(binding = 0)uniform sampler2D gBufferDiffuseTex;
layout(binding = 1)uniform sampler2D gBufferSpecularTex;
layout(binding = 2)uniform sampler2D gBufferNormalTex;
layout(binding = 3)uniform sampler2D gBufferDepthTex;


layout(binding = 4)uniform sampler2D decalDiffuseTex;
layout(binding = 5)uniform sampler2D decalSpecularTex;
//layout(binding = 6)uniform sampler2D decalNormalTex;


layout(location = 7) uniform bool hasDiffuse;
layout(location = 8) uniform bool hasSpecular;
layout(location = 9) uniform bool hasNormal;

layout(location = 10)uniform mat4 invView;
layout(location = 11) uniform mat4 invModel;


float GetLinearZ(float inputDepth)
{
	return -proj[3][2] / (proj[2][2] + (inputDepth * 2.0 - 1.0));
}

vec3 GetWorldPos(float inDepth, vec2 texCoords)
{
	float viewZ = GetLinearZ(inDepth);
	float viewX = (texCoords.x * 2.0 - 1.0) * (-viewZ) / proj[0][0];
	float viewY = (texCoords.y * 2.0 - 1.0) * (-viewZ) / proj[1][1];
	vec3 viewPos = vec3(viewX, viewY, viewZ);
	return (invView * vec4(viewPos, 1.0)).xyz;
}

in vec4 clipping;

void main()
{
	float depth = texture(gBufferDepthTex, clipping.xy/clipping.w*0.5 + 0.5).r;
	vec3 worldPos = GetWorldPos(depth, clipping.xy/clipping.w*0.5 + 0.5).xyz;
	vec3 objPos = (invModel*vec4(worldPos, 1.0)).xyz;


	if (abs(objPos.x) > 0.5 || abs(objPos.y) > 0.5 || abs(objPos.z) > 0.5)
	{
		discard;
	}
	
	vec3 normal = texture(gBufferNormalTex, clipping.xy/clipping.w*0.5 + 0.5).xyz;
	vec3 cubeZaxis;
	cubeZaxis.x = invModel[0][2];
	cubeZaxis.y = invModel[1][2];
	cubeZaxis.z = invModel[2][2];
	
	if (dot(cubeZaxis,normal) < 0.3)
	{
		discard;
	}
	//vec3 diffuseColor = texture(gBufferDiffuseTex, clipping.xy/clipping.w*0.5 + 0.5).xyz;
	//outDiffuse = diffuseColor * 100;

	vec4 diffuseDecalColor = texture(decalDiffuseTex, objPos.xy+0.5);

	if (diffuseDecalColor.w < 0.1)
	{
	 discard;
	}
	
	
	
	outDiffuse = diffuseDecalColor.rgb;
	outSpecularRough = texture(decalSpecularTex, objPos.xy+0.5);
	
	//vec4 specularColor = texture(gBufferSpecularTex, clipping.xy/clipping.w*0.5 + 0.5);
	//outSpecularRough = specularColor * 100;
	

}