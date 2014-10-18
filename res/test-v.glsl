#version 330 core
layout(location=0) in vec2 vVertex;

varying out vec2 vUV;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    vUV = position * vec2(0.5) + vec2(0.5);
}
