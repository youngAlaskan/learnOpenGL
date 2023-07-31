#version 460 core
out vec4 FragColor;

in VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} vertexData;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{
	vec3 I = normalize(vec3(vertexData.FragPos) - viewPos);
	vec3 R = reflect(I, normalize(vertexData.Normal));
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
