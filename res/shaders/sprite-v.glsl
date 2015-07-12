#version 330 core
layout(location=0) in vec3 vVertex;
/*layout(location=1) in vec2 vUV; */

uniform float vScale;
uniform mat4 mModelToClip;

/* out vec2 UV; */

void main()
{
	/*	UV = vUV; */
	vec4 vert =  mModelToClip * vec4(vVertex, 1.0);
    gl_Position = vert * vec4(vScale, vScale, vScale, 0.0);
}

