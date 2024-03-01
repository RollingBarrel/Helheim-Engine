#version 460
layout(location=0) in vec3 pos;
layout(location=1) in vec2 aTexCoord;
layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 proj;
out vec2 TexCoord;
void main()
{
	TexCoord = aTexCoord;
	gl_Position = proj*view*model*vec4(pos*1.0,1.0);
}