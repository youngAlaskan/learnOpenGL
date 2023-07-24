#version 460 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

out vec3 texCoords;

uniform mat4 proj;
uniform mat4 view;

void main()
{
	texCoords = aPos.xyz;
	gl_Position = (proj * view * aPos).xyww;
}
