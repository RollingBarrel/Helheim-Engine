#version 460 core
in vec2 texCoords;
in vec4 vertColor;
out vec4 color;

uniform sampler2D sprite;

void main()
{
    color = (ParticleColor);
}