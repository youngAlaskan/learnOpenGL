#version 460 core
out vec4 FragColor;

in VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} i_VertexData;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{
	vec3 I = normalize(vec3(i_VertexData.FragPos) - viewPos);
	vec3 R = reflect(I, normalize(i_VertexData.Normal));
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
