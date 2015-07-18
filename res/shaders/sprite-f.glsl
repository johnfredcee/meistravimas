#version 330 core
out vec4 vFragColor;

uniform sampler2D textureMap;

in vec2 UV;

void main()
{
	vFragColor = texture(textureMap, UV); 
}
