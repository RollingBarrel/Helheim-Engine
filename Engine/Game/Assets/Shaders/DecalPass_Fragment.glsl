#version 460 core

layout(location = 0) out vec3 outDiffuse;
layout(location = 1) out vec4 outSpecularRough;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outEmissive;

layout(std140, binding = 0) uniform CameraMatrices{
	mat4 view;
	mat4 proj;
};


layout(binding = 3)uniform sampler2D gBufferPositionTex;


layout(binding = 5)uniform sampler2D decalDiffuseTex;
layout(binding = 6)uniform sampler2D decalSpecularTex;
layout(binding = 7)uniform sampler2D decalNormalTex;
layout(binding = 8)uniform sampler2D decalEmisiveTex;


layout(location = 11) uniform bool hasDiffuse;
layout(location = 12) uniform bool hasSpecular;
layout(location = 13) uniform bool hasNormal;
layout(location = 14) uniform bool hasEmisive;

layout(location = 15)uniform mat4 invView;
layout(location = 16) uniform mat4 invModel;

in vec4 clipping;

void main()
{
	vec3 worldPos = texture(gBufferPositionTex, (clipping.xy/clipping.w)*0.5 + 0.5).xyz;
	vec3 objPos = (invModel*vec4(worldPos, 1.0)).xyz;
	objPos.y *= -1;

	if (abs(objPos.x) > 0.5 || abs(objPos.y) > 0.5 || abs(objPos.z) > 0.5)
	{
		discard;
	}
	

	vec3 tangent = normalize(dFdx(worldPos));
	vec3 bitangent =  normalize(dFdy(worldPos));
	vec3 normal = normalize(cross(tangent, bitangent));

	vec3 cubeZaxis;
	cubeZaxis.x = invModel[0][2];
	cubeZaxis.y = invModel[1][2];
	cubeZaxis.z = invModel[2][2];
	
	if (dot(cubeZaxis,normal) < 0.3)
	{
		discard;
	}



	if (hasDiffuse)
	{
		vec4 diffuseDecalColor = texture(decalDiffuseTex, objPos.xy+0.5);

		if (diffuseDecalColor.w < 0.1)
		{
			discard;
		}
		outDiffuse = pow(diffuseDecalColor.rgb,vec3(2.2));
	}
	
	if (hasSpecular)
	{
		vec4 specTex = texture(decalSpecularTex, objPos.xy+0.5);
		outSpecularRough = pow(specTex, vec4(2.2));
	}
	
	if (hasEmisive)
	{
		vec3 emissiveTex = texture(decalEmisiveTex, objPos.xy+0.5).rgb;
		outEmissive = pow(emissiveTex, vec3(2.2));
	}

	if (hasNormal)
	{
		outNormal = (mat3(tangent, bitangent, normal) * (texture(decalNormalTex, objPos.xy+0.5).rgb*2.0-1.0))*0.5+0.5;
		//outNormal = normal *0.5+0.5;
	}

	

	
	//vec4 specularColor = texture(gBufferSpecularTex, clipping.xy/clipping.w*0.5 + 0.5);
	//outSpecularRough = specularColor * 100;
	

}