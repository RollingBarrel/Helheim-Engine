#version 460 core
layout(binding = 0) uniform sampler2D imgTex;
layout(binding = 1) uniform sampler2D blurTex;
in vec2 uv;
out vec4 col;
layout(location = 3)uniform vec4 inputColor;
void main()
{
    if((texture(imgTex, uv).a * inputColor.a) == 0.0f)
        discard;
    vec2 uvs = gl_FragCoord.xy / textureSize(blurTex, 0);
    col = texture(blurTex, uvs);
}
