#version 460 core
out vec4 FragColor;

in vec3 ourColor;
in vec3 texCoord;

uniform samplerCube texture1;
uniform samplerCube texture2;
uniform float mixPercent;

void main()
{
    FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), mixPercent);
}