#version 460 core

layout (location = 0) in vec4 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

uniform mat4 modelInv;
uniform mat4 model;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} o_VertexData;

void main()
{
    o_VertexData.FragPos = model * a_Position;
    o_VertexData.Normal = normalize(mat3(transpose(modelInv)) * a_Normal);
    o_VertexData.TexCoords = a_TexCoords;

    gl_Position = projection * view * o_VertexData.FragPos;
} 