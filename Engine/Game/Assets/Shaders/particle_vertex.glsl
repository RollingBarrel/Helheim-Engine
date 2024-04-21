#version 460 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec4 color;
layout (location = 2) in mat4 modelMatrix;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;

void main()
{
    TexCoords = vertex.zw;
    ParticleColor = color;
    gl_Position = projection * modelMatrix * vec4((vertex.xy), 0, 1.0);
}