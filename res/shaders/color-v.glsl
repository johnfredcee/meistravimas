#version 330 core

layout(location = 0) in vec3 vVertex;

uniform mat4 mView;
uniform mat4 mProj;

void main()
{
	gl_Position = mProj * mView * vec4(vVertex, 1.0);
}
