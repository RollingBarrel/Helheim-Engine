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
	vec3 tLightDir;
	vec3 tPos;
	vec3 tCameraPos;
	vec3 tNorm;
};


//Light properties
layout (location = 3)uniform vec3 lightColor;
layout (location = 4)uniform vec3 ambientCol;
uniform (location = 5)float lightIntensity;//0-5

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
		N = normalize(texture(material.normalTexture, uv).rgb * 2.0 - 1.0);
	}
	else{
		N = normalize(tNorm);  	//Normal
	}
	vec3 L =  -normalize(tLightDir); 	//Light direction
	float NdotL = max(dot(N,L),0);	//It doesn't make sense for color to be negative
	
	vec3 R = reflect(L,N);
	vec3 V = normalize(tCameraPos - tPos); //View direction
	float VdotRpown = pow(max(dot(V,R), 0), shininess);
	
	vec3 RFOi = specularColor + (1-specularColor) * pow(1-NdotL,5);
	
	vec3 Li = lightIntensity * lightColor;  //Incoming radiance
	
	//Color with specular and no pi corretion
	vec3 pbrColor = ((diffuseColor*(1-specularColor)) + ((shininess +2)/2)* RFOi * VdotRpown) * Li * NdotL;
	
	//Final color  
	vec3 color = material.ambientColor * diffuseColor + pbrColor;
	
	//Gamma correction
	color.rgb = pow(color.rgb, vec3(1/2.2));
	
	//Output
	outColor = vec4(color, 1.0f);
	
}