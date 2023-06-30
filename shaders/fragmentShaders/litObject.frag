#version 460 core
struct Material
{
    samplerCube diffuse;
    samplerCube specular;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 color;

    float kA;
    float kD;
    float kS;
};

in vec3 normal;
in vec3 fragPos;
in vec3 color;
in vec3 texCoord;

out vec4 FragColor;
  
uniform Material material;
uniform Light light;

uniform vec3 viewPos;

void main()
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);

    float lambertian = max(dot(norm, lightDir), 0.0);
    float spec = 0.0;
    if (lambertian > 0.0)
    {
        vec3 reflected = reflect(-lightDir, normal);
        vec3 toViewer = normalize(viewPos - fragPos);

        float specAngle = max(dot(toViewer, reflected), 0.0);
        spec = pow(specAngle, material.shininess);
    }

    vec3 ambient  = light.kA * texture(material.diffuse, texCoord).rgb;
    vec3 diffuse  = light.kD * lambertian * texture(material.diffuse, texCoord).rgb;
    vec3 specular = light.kS * (spec * texture(material.specular, texCoord).rgb);

    FragColor = vec4((ambient + diffuse + specular) * light.color, 1.0);
}