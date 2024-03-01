#version 460
layout (location = 5) uniform sampler2D Texture;
in vec2 uv0;
out vec4 color;
void main()
{
 color = texture(Texture, uv0);
}