#version 460
layout (location = 5) uniform sampler2D Texture;
uniform vec4 inputColor;
in vec2 uv0;
out vec4 fragment_color;
void main()
{
 fragment_color = texture(Texture, uv0) * inputColor;
 if(fragment_color.a < 0.1) discard;
}
