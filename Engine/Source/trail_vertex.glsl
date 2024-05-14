#version 460 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texCoord
layout (location = 2) in vec4 color;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 viewProj;

void main()
{
    texCoords = texCoord;
    vertColor = color;
    gl_Position = viewProj * vec4(vertex, 1.0);
}