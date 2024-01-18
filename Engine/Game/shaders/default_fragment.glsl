#version 440

in vec4 color;

in vec3 position;
in vec3 normal;
//in vec2 uv0;

//Light properties
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float lightIntensity;

uniform vec3 cameraPos;

uniform int hasDiffuseMap;
uniform int hasSpecularMap;
uniform int hasShininessMap;

uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform int shininess;
uniform vec3 ambientColor;

vec3 diffuse;
vec3 specular;
int shiny;

out vec4 outColor;

void main() {

	//Diffuse
	if(hasDiffuseMap == 1){//Using  gamma correction forces to transform sRGB textures to linear space
		//diffuse = vec3(texture(diffuseTexture, uv0));
		//diffuse = pow(diffuseColor, vec3(2.2));
		diffuse = vec3(254.0f/255.0f,1.0f/255.0f,154.0f/255.0f);
	}else{
		diffuse = diffuseColor.xyz;
	}
	//Specular
	if(hasSpecularMap == 1){//Using  gamma correction forces to transform sRGB textures to linear space
		//specular = vec3(texture(specularTexture, uv0));
		//specular = pow(specularColor,vec3(2.2));
		specular = vec3(0,0,0);
	}else{
		specular = specularColor;
	}
	//Shininess
	if(hasShininessMap == 1){
		//shiny = exp2(15*texture(specularTexture, uv0).a+1);
		shiny = 0;
	}else{
		shiny = shininess;
	}

	vec3 N = -normalize(normal);  	//Normal
	vec3 L =  normalize(lightDir); 	//Light direction
	float NdotL = max(dot(N,L),0);	//It doesn't make sense for color to be negative
	
	vec3 R = reflect(L,N);
	vec3 V = normalize(cameraPos - position); //View direction
	float VdotRpown = pow(max(dot(V,R), 0), shiny);
	
	vec3 RFOi = specular + (1-specular) * pow(1-NdotL,5);
	
	vec3 Li = lightIntensity * lightColor;  //Incoming radiance

	//Color with specular and no pi corretion
	vec3 shaderColor = ((diffuse*(1-specular)) + ((shiny +2)/2)* RFOi * VdotRpown) * Li * NdotL;
	
	//Final color
	vec3 finalColor = ambientColor * diffuse + shaderColor;
	
	//Gamma correction
	finalColor.rgb = pow(finalColor.rgb, vec3(1/2.2));
	
	//Output
	outColor = vec4(finalColor, 1.0f);

}