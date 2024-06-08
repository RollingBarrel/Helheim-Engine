#version 460 core

//out vec4 albedo;
layout(location = 0) out vec3 outDiffuse;
layout(location = 1) out vec4 outSpecularRough;
layout(location = 2) out vec3 outNormal;
layout(location = 4) out vec3 outEmissive;


layout(binding = 0)uniform sampler2D gBufferDiffuseTex;
//layout(binding = 1)uniform sampler2D gBufferSpecularTex;
//layout(binding = 2)uniform sampler2D gBufferNormalTex;


layout(binding = 3)uniform sampler2D decalDiffuseTex;
//layout(binding = 4)uniform sampler2D decalSpecularTex;
//layout(binding = 5)uniform sampler2D decalNormalTex;


layout(location = 6) uniform bool hasDiffuse;
layout(location = 7) uniform bool hasSpecular;
layout(location = 8) uniform bool hasNormal;
layout(location = 9) uniform mat4 invModel;


in vec4 clipping;

void main()
{
	vec3 worldPos = texture(gBufferDiffuseTex, clipping.xy/clipping.w*0.5 + 0.5).xyz;
	vec3 objPos = (invModel*vec4(worldPos, 1.0)).xyz;
	if (abs(objPos.x) > 0.5 || abs(objPos.y) > 0.5 || abs(objPos.z) > 0.5)
	{
		discard;
	}

	outDiffuse = texture(decalDiffuseTex, objPos.xy+0.5).rgb;

}