#version 460
struct Material
{
    samplerCube diffuse;
    samplerCube specular;
    samplerCube emissive;
    float shininess;
};

struct DirLight
{
	vec3 direction;
    vec3 color;

	float kA;
    float kD;
    float kS;
};

struct PointLight
{
    vec3 position;
    vec3 color;

    float kA;
    float kD;
    float kS;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
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

    float innerCutOff;
    float outerCutOff;
};

uniform Material material;

uniform DirLight dirLight;
#define POINT_LIGHT_COUNT 1
uniform PointLight pointLights[POINT_LIGHT_COUNT];
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform vec3 viewPos;

in vec3 normal;
in vec3 fragPos;
in vec3 color;
in vec3 texCoord;

out vec4 FragColor;

void main()
{
	vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = vec3(0.0);
    if (dirLight.direction != vec3(0.0, 0.0, 0.0))
        result += CalcDirLight(dirLight, norm, fragPos, viewDir);

    for (int i = 0; i < POINT_LIGHT_COUNT; i++)
    {
        result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
    }

    if (spotLight.innerCutOff != 0.0)
        result += CalcSpotLight(spotLight, norm, fragPos, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

    float lambertian = max(dot(normal, lightDir), 0.0);
    float spec = 0.0;
    if (lambertian > 0.0)
    {
        vec3 reflected = reflect(-lightDir, normal);
        vec3 toViewer = normalize(viewPos - fragPos);

        float specAngle = max(dot(toViewer, reflected), 0.0);
        spec = pow(specAngle, material.shininess * 128.0);
    }

    vec3 ambient  = light.kA * texture(material.diffuse, texCoord).rgb;
    vec3 diffuse  = light.kD * lambertian * texture(material.diffuse, texCoord).rgb;
    vec3 specular = light.kS * (spec * texture(material.specular, texCoord).rgb);
    vec3 emissive = texture(material.emissive, texCoord).rgb;

    return (ambient + diffuse + specular + emissive) * light.color;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);

    float lambertian = max(dot(norm, lightDir), 0.0);
    float spec = 0.0;
    if (lambertian > 0.0)
    {
        vec3 reflected = reflect(-lightDir, normal);
        vec3 toViewer = normalize(viewPos - fragPos);

        float specAngle = max(dot(toViewer, reflected), 0.0);
        spec = pow(specAngle, material.shininess * 128.0);
    }

    vec3 ambient  = attenuation * light.kA * texture(material.diffuse, texCoord).rgb;
    vec3 diffuse  = attenuation * light.kD * lambertian * texture(material.diffuse, texCoord).rgb;
    vec3 specular = attenuation * light.kS * (spec * texture(material.specular, texCoord).rgb);
    vec3 emissive = texture(material.emissive, texCoord).rgb;

    return (ambient + diffuse + specular + emissive) * light.color;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{

    vec3 fragToLight = normalize(light.position - fragPos);

    float phi = dot(fragToLight, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((phi - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 norm = normalize(normal);

    float lambertian = max(dot(norm, fragToLight), 0.0);
    float spec = 0.0;
    if (lambertian > 0.0)
    {
        vec3 reflected = reflect(-fragToLight, normal);
        vec3 toViewer = normalize(viewPos - fragPos);

        float specAngle = max(dot(toViewer, reflected), 0.0);
        spec = pow(specAngle, material.shininess * 128.0);
    }
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient  = light.kA * texture(material.diffuse, texCoord).rgb;
    vec3 diffuse  = intensity * attenuation * light.kD * lambertian * texture(material.diffuse, texCoord).rgb;
    vec3 specular = intensity * attenuation * light.kS * (spec * texture(material.specular, texCoord).rgb);
    vec3 emissive = texture(material.emissive, texCoord).rgb;

    return (ambient + diffuse + specular + emissive) * light.color;
}
