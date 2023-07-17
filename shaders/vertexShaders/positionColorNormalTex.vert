#version 460 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

uniform mat4 modelInv;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 fragPos;
out vec4 color;
out vec3 normal;
out vec2 texCoord;

void main()
{
    fragPos = model * aPos;
    normal = mat3(transpose(modelInv)) * aNormal;
    color = aColor;
    texCoord = aTexCoord;

    gl_Position = proj * view * fragPos;
} 