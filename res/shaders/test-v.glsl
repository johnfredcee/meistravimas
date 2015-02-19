#version 330 core
layout(location=0) in vec2 vVertex;

out vec2 vUV;

void main()
{
    gl_Position = vec4(vVertex, 0.0, 1.0);
    vUV = vVertex * vec2(0.5) + vec2(0.5);
	vUV.y = 1.0 - vUV.y;
}
