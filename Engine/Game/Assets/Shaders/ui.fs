#version 460 core

uniform sampler2D Texture;
uniform vec4 inputColor;

in vec2 uv0;
out vec4 fragmentColor;

void main()
{
    fragmentColor = texture(Texture, uv0) * inputColor;
    if(fragmentColor.a < 0.1) discard;
}
