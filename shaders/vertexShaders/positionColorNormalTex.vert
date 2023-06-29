#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTexCoord;

uniform mat4 modelInv;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 normal;
out vec3 fragPos;
out vec3 color;
out vec3 texCoord;

void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0f);
    fragPos = mat3(model) * aPos;
    normal = mat3(transpose(modelInv)) * aNormal;
    color = aColor;
    texCoord = aTexCoord;
} 