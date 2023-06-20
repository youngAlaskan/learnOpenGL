#version 460 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec4 aTexCoord;

out vec4 ourColor;
out vec4 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * model * aPos;
    ourColor = aColor;
    texCoord = aTexCoord;
}