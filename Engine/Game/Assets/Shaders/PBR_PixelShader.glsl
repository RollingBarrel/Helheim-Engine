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
layout (location = 1)uniform vec3 lDir;
layout (location = 2)uniform vec3 cPos;
layout (location = 3) uniform vec3 lightColor;
layout (location = 4) uniform vec3 ambientColor;
layout (location = 5) uniform float lightIntensity;//0-5

uniform Material material;

out vec4 outColor;

void main() {

	vec3 diffuseColor;
	vec3 specularColor;
	float shininess;

	//Diffuse
	if(material.hasDiffuseMap){//Using  gamma correction forces to transform sRGB textures to linear space
		diffuseColor = vec3(texture(material.diffuseTexture, uv));
		diffuseColor = pow(diffuseColor, vec3(2.2));
	}else{
		diffuseColor = material.diffuseColor;
	}
	//Specular
	if(material.hasSpecularMap){//Using  gamma correction forces to transform sRGB textures to linear space
		specularColor = vec3(texture(material.specularTexture, uv));
		specularColor = pow(specularColor,vec3(2.2));
	}else{
		specularColor = material.specularColor;
	}
	//Shininess
	if(material.hasShininessMap){
		shininess = exp2(15*texture(material.specularTexture, uv).a+1);
	}else{
		shininess = material.shininess;
	}
	vec3 N = vec3(0);
	if (material.hasNormalMap){
		N = normalize(norm);
		vec3 T = normalize(tang.xyz); 
		vec3 B = tang.w * cross(N, T);
		mat3 TBN = mat3(T,B,N);
		N = normalize(texture(material.normalTexture, uv).rgb * 2.0 - 1.0);
		N = normalize(TBN * N);
	}
	else{
		N = normalize(norm);  	//Normal
	}
	vec3 L =  -normalize(lDir); 	//Light direction
	float NdotL = max(dot(N,L),0);	//It doesn't make sense for color to be negative
	
	vec3 R = reflect(L,N);
	vec3 V = normalize(cPos - sPos); //View direction
	float VdotRpown = pow(max(dot(V,R), 0), shininess);
	
	vec3 RFOi = specularColor + (1-specularColor) * pow(1-NdotL,5);
	
	vec3 Li = lightIntensity * lightColor;  //Incoming radiance
	
	//Color with specular and no pi division
	//vec3 pbrColor = ((diffuseColor*(1-specularColor)) + ((shininess+2)/2)* RFOi * VdotRpown) * Li * NdotL;
	//Color with specular and pi divisions
	float pi = 3.1415926535897932384626433832795;
	vec3 pbrColor = ((diffuseColor*(1-specularColor))/pi + ((shininess+2)/2*pi)* RFOi * VdotRpown) * Li * NdotL;
	
	//Final color  
	vec3 color = ambientColor * diffuseColor + pbrColor;
	
	//Gamma correction
	color.rgb = pow(color.rgb, vec3(1/2.2));
	
	//Output
	outColor = vec4(color, 1.0f);
	
}