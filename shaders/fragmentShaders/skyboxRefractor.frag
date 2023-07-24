#version 460 core
out vec4 FragColor;

in vec3 normal;
in vec4 fragPos;

uniform float refractiveIndex;
uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{
	float ratio = 1.00 / refractiveIndex;
	vec3 I = normalize(vec3(fragPos) - viewPos);
	vec3 R = refract(I, normalize(normal), ratio);
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
