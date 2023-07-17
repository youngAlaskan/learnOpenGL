#version 460 core
out vec4 FragColor;

in vec4 fragPos;
in vec4 color;
in vec3 normal;
in vec2 texCoord;

void main()
{
    FragColor = color;
}