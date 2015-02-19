#version 330 core

layout(location = 0) in vec2 vVertex;

uniform mat4 mPanel;

void main()
{
	gl_Position = mPanel * vec4(vVertex, 0.0, 1.0);
}
