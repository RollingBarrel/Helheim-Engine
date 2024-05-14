#version 460 core

in vec3 texcoords;
out vec4 color;
uniform samplerCube cube;


void main()
{
    vec3 envColor = texture(cube, texcoords).rgb;
    
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); //Inverse Gamma Correction
    
    color = vec4(envColor, 1.0);
}