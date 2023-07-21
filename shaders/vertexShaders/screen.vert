#version 460 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

out vec2 texCoords;

void main()
{
	texCoords = aTexCoords;
	gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
