#version 440
layout(location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
//layout(location = 2) in vec2 vertex_uv0;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 viewProj;

//out vec4 color;
out vec3 position;
out vec3 normal;
//out vec2 uv0;

void main()
{
	gl_Position = viewProj * model * vec4(vertexPosition, 1.0);
	position = (model * vec4(vertexPosition, 1.0f)).xyz;
	normal = transpose(inverse(mat3(model))) * vertexNormal;
	//uv0 = vertexUV0;

	//color = vec4(abs(normalize(normal)), 1.0);
}