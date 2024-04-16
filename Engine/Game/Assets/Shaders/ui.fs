#version 460
layout (location = 5) uniform sampler2D Texture;
uniform vec4 inputColor;
in vec2 uv0;
out vec4 color;
void main()
{
 color = inputColor * texture(Texture, uv0);
}
