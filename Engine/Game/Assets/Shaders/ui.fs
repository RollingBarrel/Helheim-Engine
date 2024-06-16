#version 460 core

layout (binding = 0) uniform sampler2D Texture;
layout (binding = 1) uniform sampler2D maskTexture;

uniform vec4 inputColor;
uniform bool useMask;
uniform vec2 textureOffset;

in vec2 uv0;
out vec4 fragment_color;

void main()
{
    vec2 adjustedUV = uv0 + textureOffset;
    vec4 texColor = texture(Texture, adjustedUV) * inputColor; 
    if (useMask) {
        vec4 maskColor = texture(maskTexture, uv0);
        texColor.a *= maskColor.a;
    }
    fragment_color = texColor;
}

//#version 460 core 

//layout (binding = 0) uniform sampler2D Texture; 

//uniform vec4 inputColor; 

//in vec2 uv0; 
//out vec4 fragment_color; 

//void main() 
//{ 
//    fragment_color = texture(Texture, uv0) * inputColor; 
//}
