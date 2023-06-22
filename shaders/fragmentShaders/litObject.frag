#version 460 core
struct Material
{
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
};

struct Light
{
    vec3 position;

    float kA;
    float kD;
    float kS;
};

in vec3 normal;
in vec3 fragPos;
in vec3 color;

out vec4 FragColor;
  
uniform Material material;
uniform Light light;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);


    float lambertian = max(dot(norm, lightDir), 0.0);
    float spec = 0.0;
    if (lambertian > 0.0)
    {
        vec3 reflected = reflect(-lightDir, normal);
        vec3 toViewer = normalize(viewPos - fragPos);

        float specAngle = max(dot(toViewer, reflected), 0.0);
        spec = pow(specAngle, material.shininess);
    }

    vec3 ambient  = light.kA * material.ambientColor;
    vec3 diffuse  = light.kD * lambertian * material.diffuseColor;
    vec3 specular = light.kS * spec * material.specularColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}