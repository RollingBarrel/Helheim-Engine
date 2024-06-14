#version 460 core

layout(location = 0) out vec4 outDiffuse;
layout(location = 1) out vec4 outSpecularRough;
layout(location = 2) out vec4 outNormal;
layout(location = 3) out vec4 outEmissive;

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

layout(location = 15) uniform mat4 invView;
layout(location = 16) uniform mat4 invModel;


layout(location = 45) uniform vec4 diffuseColor;
layout(location = 46) uniform vec3 emisiveColor;
layout(location = 47) uniform bool isSpriteSheet;
//layout(location = 48) uniform vec2 spriteSheetSize;
//layout(location = 49) uniform vec2 spriteSheetCurrentPosition;
layout(location = 49) uniform vec2 spriteSheetStart;
layout(location = 48) uniform vec2 spriteSheetOffset;

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


	//float U = 1.0 / spriteSheetSize.x;
	//float V = 1.0 / spriteSheetSize.y;
	
	//float offsetU = (objPos.x+0.5)/U;
	//offsetU = U * fract(offsetU) + floor(offsetU) * U;
	
	
	//float uStart = spriteSheetCurrentPosition.y / spriteSheetSize.y;
	//float vStart = spriteSheetCurrentPosition.x / spriteSheetSize.x;
	//
	//float uEnd = uStart + (1.0 / spriteSheetSize.y);
	//float vEnd = vStart + (1.0 / spriteSheetSize.x);
	//
	//
	//
	//float offsetU = (uStart + uEnd) * (objPos.x+0.5); 
	//float offsetV = (vStart + vEnd) * (objPos.y+0.5); 

	vec2 texCoords = {spriteSheetOffset.x * (objPos.x+0.5) + spriteSheetStart.x , spriteSheetOffset.y * (objPos.y+0.5) + spriteSheetStart.y };

	float metal = 0.01;
	float rough = 0.99;


	if (hasSpecular)
	{
		vec2 metRough = texture(decalSpecularTex, texCoords).gb;
		metal = metRough.y;
		rough = metRough.x;
		
		outSpecularRough.rgb = mix(vec3(0.04), diffuseColor.rgb, metal);
		outSpecularRough.a = rough;
		
		//outSpecularRough = pow(specTex, vec4(2.2));
		//outSpecularRough.w = 1.0f;
	}



	if (hasDiffuse)
	{
		vec4 diffuseDecalColor = texture(decalDiffuseTex, texCoords); // * diffuseColor;
		diffuseDecalColor.rgb =  pow(diffuseDecalColor.rgb,vec3(2.2));
		diffuseDecalColor *= diffuseColor;	
	
		if (diffuseDecalColor.w < 0.1)
		{
			//discard;
		}

		outDiffuse = diffuseDecalColor * (1 - metal);
		outDiffuse.w = 1.0;
	}
	
	
	
	if (hasEmisive)
	{
		vec3 emissiveTex = texture(decalEmisiveTex, texCoords).rgb * emisiveColor;
		outEmissive.rgb = pow(emissiveTex, vec3(2.2));
		outEmissive.w = 1.0;
	}

	if (hasNormal)
	{
		outNormal.rgb = (mat3(tangent, bitangent, normal) * (texture(decalNormalTex, texCoords).rgb*2.0-1.0))*0.5+0.5;
		outNormal.w = 1.0;
		//outNormal = normal *0.5+0.5;
	}

	

	
	//vec4 specularColor = texture(gBufferSpecularTex, clipping.xy/clipping.w*0.5 + 0.5);
	//outSpecularRough = specularColor * 100;
	

}