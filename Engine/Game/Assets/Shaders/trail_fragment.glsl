#version 460 core
in vec2 TexCoords;
in vec4 VertexColor;
out vec4 color;

uniform sampler2D sprite;

void main()
{
    color = (texture(sprite, TexCoords) * VertexColor);
}