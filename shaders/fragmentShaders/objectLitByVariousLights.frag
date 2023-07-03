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

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir, mat3 texValues);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, mat3 texValues);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, mat3 texValues);

float CalcSpec(vec3 fragToLight);

uniform vec3 viewPos;

in vec3 normal;
in vec3 fragPos;
in vec3 color;
in vec3 texCoord;

out vec4 FragColor;

void main()
{
    mat3 texValues = mat3(
        texture(material.diffuse, texCoord).rgb,
        texture(material.specular, texCoord).rgb,
        texture(material.emissive, texCoord).rgb);

	vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = vec3(0.0);
    if (dirLight.direction != vec3(0.0, 0.0, 0.0))
        result += CalcDirLight(dirLight, norm, fragPos, viewDir, texValues);

    for (int i = 0; i < POINT_LIGHT_COUNT; i++)
    {
        result += CalcPointLight(pointLights[i], norm, fragPos, viewDir, texValues);
    }

    if (spotLight.innerCutOff != 0.0)
        result += CalcSpotLight(spotLight, norm, fragPos, viewDir, texValues);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir, mat3 texValues)
{
	vec3 fragToLight = normalize(-light.direction);

    float lambertian = max(dot(normal, fragToLight), 0.0);
    float spec = lambertian > 0.0 ? CalcSpec(fragToLight) : 0.0;

    vec3 ambient  = texValues[0].rgb * light.kA;
    vec3 diffuse  = texValues[0].rgb * light.kD * lambertian;
    vec3 specular = texValues[1].rgb * light.kS * spec;
    vec3 emissive = texValues[2].rgb;

    return (ambient + diffuse + specular + emissive) * light.color;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, mat3 texValues)
{
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 fragToLight = normalize(light.position - fragPos);

    float lambertian = max(dot(normal, fragToLight), 0.0);
    float spec = lambertian > 0.0 ? CalcSpec(fragToLight) : 0.0;

    vec3 ambient  = texValues[0].rgb * light.kA * attenuation;
    vec3 diffuse  = texValues[0].rgb * light.kD * attenuation * lambertian;
    vec3 specular = texValues[1].rgb * light.kS * attenuation * spec;
    vec3 emissive = texValues[2].rgb;

    return (ambient + diffuse + specular + emissive) * light.color;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, mat3 texValues)
{

    vec3 fragToLight = normalize(light.position - fragPos);

    float phi = dot(fragToLight, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((phi - light.outerCutOff) / epsilon, 0.0, 1.0);

    float lambertian = max(dot(normal, fragToLight), 0.0);
    float spec = lambertian > 0.0 ? CalcSpec(fragToLight) : 0.0;
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient  = texValues[0].rgb * light.kA;
    vec3 diffuse  = texValues[0].rgb * light.kD * intensity * attenuation * lambertian;
    vec3 specular = texValues[1].rgb * light.kS * intensity * attenuation * spec;
    vec3 emissive = texValues[2].rgb;

    return (ambient + diffuse + specular + emissive) * light.color;
}

float CalcSpec(vec3 fragToLight)
{
    vec3 reflected = reflect(-fragToLight, normal);
    vec3 toViewer = normalize(viewPos - fragPos);

    float specAngle = max(dot(toViewer, reflected), 0.0);
    return pow(specAngle, material.shininess * 128.0);
}
