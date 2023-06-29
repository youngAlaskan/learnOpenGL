#version 460 core
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;
in vec3 color;
in vec3 texCoord;

void main()
{
    FragColor = vec4(color, 1.0);
}