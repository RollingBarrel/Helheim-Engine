#version 440

struct Material
{
	//Diffuse
	vec3 diffuseColor;
	sampler2D diffuseTexture;
	//Specular
	vec3 specularColor;
	sampler2D specularTexture;
	
	float shininess; //Shininess
	vec3 ambientColor; //Ambient color
	
	//Options
	bool hasDiffuseMap;
	bool hasSpecularMap;
	bool hasShininessMap;
};

in vec3 position;
in vec3 normal;
in vec2 uv0;

//Light properties
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float lightIntensity;

uniform vec3 cameraPos;

uniform Material material;

vec3 diffuseColor;
vec3 specularColor;
float shininess;

out vec4 outColor;

void main() {

	//Diffuse
	if(material.hasDiffuseMap){//Using  gamma correction forces to transform sRGB textures to linear space
		diffuseColor = vec3(texture(material.diffuseTexture, uv0));
		diffuseColor = pow(diffuseColor, vec3(2.2));
	}else{
		diffuseColor = material.diffuseColor;
	}
	//Specular
	if(material.hasSpecularMap){//Using  gamma correction forces to transform sRGB textures to linear space
		specularColor = vec3(texture(material.specularTexture, uv0));
		specularColor = pow(specularColor,vec3(2.2));
	}else{
		specularColor = material.specularColor;
	}
	//Shininess
	if(material.hasShininessMap){
		shininess = exp2(15*texture(material.specularTexture, uv0).a+1);
	}else{
		shininess = material.shininess;
	}
	
	vec3 N = -normalize(normal);  	//Normal
	vec3 L =  normalize(lightDir); 	//Light direction
	float NdotL = max(dot(N,L),0);	//It doesn't make sense for color to be negative
	
	vec3 R = reflect(L,N);
	vec3 V = normalize(cameraPos - position); //View direction
	float VdotRpown = pow(max(dot(V,R), 0), shininess);
	
	vec3 RFOi = specularColor + (1-specularColor) * pow(1-NdotL,5);
	
	vec3 Li = lightIntensity * lightColor;  //Incoming radiance
	
	//Color with specular and no pi corretion
	vec3 lightColor = ((diffuseColor*(1-specularColor)) + ((shininess +2)/2)* RFOi * VdotRpown) * Li * NdotL;
	
	//Final color  
	vec3 color = material.ambientColor * diffuseColor + lightColor;
	
	//Gamma correction
	color.rgb = pow(color.rgb, vec3(1/2.2));
	
	//Output
	outColor = vec4(color, 1.0f);
	
}