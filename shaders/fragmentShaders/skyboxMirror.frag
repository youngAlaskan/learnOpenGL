#version 460 core
out vec4 FragColor;

in vec3 normal;
in vec4 fragPos;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{
	vec3 I = normalize(vec3(fragPos) - viewPos);
	vec3 R = reflect(I, normalize(normal));
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
