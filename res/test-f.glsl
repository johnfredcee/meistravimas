#version 330 core
layout (location=0) out vec4 vFragColor;

uniform sampler2D textureMap;

varying in vec2 vUV;

void main()
{
	vFragColor = texture(textureMap, vUV);
}
