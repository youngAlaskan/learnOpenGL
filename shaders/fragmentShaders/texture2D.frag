#version 460

uniform sampler2D tex;

in vec2 texCoords;

out vec4 FragColor;

void main()
{
	FragColor = vec4(texture(tex, texCoords).rgb, 1.0);
}