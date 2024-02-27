#version 460 core

struct Material
{
	//Diffuse
	vec3 diffuseColor;
	sampler2D diffuseTexture;
	//Specular
	vec3 specularColor;
	sampler2D specularTexture;
	//Normal
	sampler2D normalTexture;
	
	float shininess; //Shininess
	//vec3 ambientColor; //Ambient color
	
	//Options
	bool hasDiffuseMap;
	bool hasSpecularMap;
	bool hasShininessMap;
	bool hasNormalMap;
};

in VertToFrag {
	vec2 uv;
	vec3 sPos;
	vec3 norm;
	vec4 tang;
};

//Light properties
layout(std140, binding = 1) uniform DirAmbientLights {
	vec3 dirDir;
	vec4 dirCol; //w is the intensity (0-5)
	vec3 ambientCol;
};
layout (location = 1)uniform vec3 cPos;
struct PointLight{
	vec4 pos; //w is the radius
	vec4 col;//a is intensity
};
readonly layout(std430, binding = 0) buffer PointLights
{
	uint numPLights;
	PointLight pLights[];
};

uniform Material material;

out vec4 outColor;

#define PI 3.1415926535897932384626433832795
vec3 diffuseColor;
vec3 specularColor;
float shininess;
vec3 V;
vec3 N;
vec3 GetPBRLightColor(vec3 lDir, vec3 lCol, float lInt, float lAtt)
{
	
	vec3 L =  -normalize(lDir); 	//Light direction
	float NdotL = max(dot(N,L),0);	//It doesn't make sense for color to be negative
	
	vec3 R = reflect(L,N);
	float VdotRpown = pow(max(dot(V,R), 0), shininess);
	
	vec3 RFOi = specularColor + (1-specularColor) * pow(1-NdotL,5);
	
	vec3 Li = lInt * lAtt * lCol.rgb;  //Incoming radiance
	
	//Color with specular and no pi division
	//vec3 pbrColor = ((diffuseColor*(1-specularColor)) + ((shininess+2)/2)* RFOi * VdotRpown) * Li * NdotL;
	//Color with specular and pi divisions
	vec3 pbrColor = ((diffuseColor*(1-specularColor))/PI + ((shininess+2)/2*PI)* RFOi * VdotRpown) * Li * NdotL;
	return pbrColor;
}

void main() {

	//Diffuse
	if(material.hasDiffuseMap)
	{
		//Using  gamma correction forces to transform sRGB textures to linear space
		diffuseColor = vec3(texture(material.diffuseTexture, uv));
		diffuseColor = pow(diffuseColor, vec3(2.2));
	}
	else
	{
		diffuseColor = material.diffuseColor;
	}
	//Specular
	vec4 specularTex = (0.0f);
	if(material.hasSpecularMap)
	{
		specularTex = texture(material.specularTexture, uv);
		specularColor = specularTex.xyz;
		//Using  gamma correction forces to transform sRGB textures to linear space
		specularColor = pow(specularColor,vec3(2.2));
	}
	else
	{
		specularColor = material.specularColor;
	}
	//Shininess
	if(material.hasShininessMap)
	{
		if(material.hasSpecularMap)
			shininess = exp2(15*specularTex.a + 1);
		else
			shininess = exp2(15*texture(material.specularTexture, uv).a+1);
	}
	else
	{
		shininess = material.shininess;
	}
	if (material.hasNormalMap)
	{
		N = normalize(norm);
		vec3 T = normalize(tang.xyz); 
		vec3 B = tang.w * cross(N, T);
		mat3 TBN = mat3(T,B,N);
		N = normalize(texture(material.normalTexture, uv).rgb * 2.0 - 1.0);
		N = normalize(TBN * N);
	}
	else
	{
		N = normalize(norm);  	//Normal
	}
	V = normalize(cPos - sPos); //View direction
	
	vec3 pbrCol = vec3(0);
	pbrCol += GetPBRLightColor(dirDir.xyz, dirCol.xyz, dirCol.w, 1);
	
	//Point lights
	for(int i = 0; i<numPLights; ++i)
	{
		vec3 mVector = sPos - pLights[i].pos.xyz;
		float dist = length(mVector);
		vec3 pDir = normalize(mVector);
		float att = pow(max(1 - pow(dist/pLights[i].pos.w,4), 0),2) / (dist*dist + 1);
		pbrCol += GetPBRLightColor(pDir, pLights[i].col.xyz,  pLights[i].col.w, att);
	}

	//HDR color  
	vec3 hdrCol = ambientCol * diffuseColor + pbrCol;
	
	//LDR color with reinhard tone Mapping
	vec3 ldrCol = hdrCol / (hdrCol.rgb + vec3(1.0));;

	//Gamma correction
	ldrCol = pow(ldrCol, vec3(1/2.2));
	
	//Output
	outColor = vec4(ldrCol, 1.0f);
}