#version 460 core
layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec3 texCoords;

void main()
{
	texCoords = aPosition.xyz;
	gl_Position = (projection * mat4(mat3(view)) * aPosition).xyww;
}
