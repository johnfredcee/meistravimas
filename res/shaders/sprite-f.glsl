#version 330 core
out vec4 vFragColor;

/* uniform sampler2D textureMap; */

in vec2 UV;

void main()
{
	vFragColor = vec4(1.0, 0.0f, 0.0, 0.0f); /* texture(textureMap, UV); */
}
