#version 460 core
layout (binding = 0) uniform sampler2D tex;
uniform vec4 inputColor;
in vec2 uv;
out vec4 col;
void main()
{
    col = texture(tex, uv) * inputColor;
}
