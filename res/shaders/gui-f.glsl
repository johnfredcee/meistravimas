#version 333 core

layout(location=0) out vec4 vFragColor;

uniform vec4 vColor;

void main()
{
	vFragColor = vColor;
}
