#version 460 core

layout(binding = 0) uniform sampler2D Texture;

uniform vec4 inputColor;

in vec2 uv0;
out vec4 fragment_color;

void main()
{
    vec4 texColor = texture(Texture, uv0);

    if (texColor.a < 0.5) discard;

    fragment_color = texColor * inputColor;
}