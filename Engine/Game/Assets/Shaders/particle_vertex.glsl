#version 460 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in mat4 matrix;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform vec4 color;

void main()
{
    float scale = 1.0f;
    TexCoords = vertex.zw;
    ParticleColor = color;
    gl_Position = matrix * projection * vec4((vertex.xy * scale) + offset.xy, offset.z, 1.0);
}