#version 460 core
out vec4 FragColor;

in vec4 ourColor;
in vec4 texCoord;

uniform samplerCube texture1;
uniform samplerCube texture2;
uniform float mixPercent;

void main()
{
    FragColor = mix(texture(texture1, texCoord.stp), texture(texture2, texCoord.stp), mixPercent);
}