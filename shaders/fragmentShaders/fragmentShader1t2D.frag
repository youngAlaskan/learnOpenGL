#version 460 core
out vec4 FragColor;

in vec4 ourColor;
in vec4 texCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, texCoord.st);
}