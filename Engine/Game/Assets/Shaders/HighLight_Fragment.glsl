#version 460 core

layout(location = 5) out vec4 FragColor;

in VertToFrag {
	vec2 uv;
	vec3 sPos;
	vec3 norm;
	vec4 tang;
	flat uint instace_index;
};

void main()
{
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}