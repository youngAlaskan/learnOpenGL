#version 460 core
struct Material
{
    samplerCube diffuse;
    samplerCube specular;
    samplerCube emissive;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 direction;
    vec3 color;

    float kA;
    float kD;
    float kS;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
};
  
uniform Material material;
uniform Light light;

uniform vec3 viewPos;

in vec3 normal;
in vec3 fragPos;
in vec3 color;
in vec3 texCoord;

out vec4 FragColor;

void main()
{

    vec3 lightDir = normalize(light.position - fragPos);

    float theta = dot(lightDir, normalize(-light.direction));

    if (theta > light.cutOff)
    {
        vec3 norm = normalize(normal);

        float lambertian = max(dot(norm, lightDir), 0.0);
        float spec = 0.0;
        if (lambertian > 0.0)
        {
            vec3 reflected = reflect(-lightDir, normal);
            vec3 toViewer = normalize(viewPos - fragPos);

            float specAngle = max(dot(toViewer, reflected), 0.0);
            spec = pow(specAngle, material.shininess * 128.0);
        }

        
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        vec3 ambient  = attenuation * light.kA * texture(material.diffuse, texCoord).rgb;
        vec3 diffuse  = attenuation * light.kD * lambertian * texture(material.diffuse, texCoord).rgb;
        vec3 specular = attenuation * light.kS * (spec * texture(material.specular, texCoord).rgb);
        vec3 emissive = texture(material.emissive, texCoord).rgb;

        FragColor = vec4((ambient + diffuse + specular + emissive) * light.color, 1.0);
    }
    else
    {
        FragColor = vec4(light.kA * vec3(texture(material.diffuse, texCoord)), 1.0);
    }

    FragColor = vec4(lightDir, 1.0);
}