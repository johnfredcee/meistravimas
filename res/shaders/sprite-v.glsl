#version 330 core
layout(location=0) in vec2 vVertex;
layout(location=1) in vec2 vUV;

uniform vec2 vScale;

out vec2 UV;

void main()
{
	UV = vUV;
	vec2 vert = vVertex * vScale;
    gl_Position = vec4(vert, 0.0, 1.0);
}

