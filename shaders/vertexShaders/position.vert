#version 460 core
layout (location = 0) in vec4 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * model * aPos;
} 