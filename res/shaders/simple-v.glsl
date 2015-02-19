#version 330 core

layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec3 vColor;

smooth out vec4 vSmoothColor;

uniform mat4 mModelToClip;

void main()
{
	vSmoothColor = vec4(vColor,1);
	gl_Position = mModelToClip * vec4(vVertex.x, vVertex.y, vVertex.z,1);
}
