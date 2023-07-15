#version 460

#define TEXTURE_CAPACITY 16
#define POINT_LIGHT_COUNT 1

struct Material
{
    int diffuseEnd;
    int specularEnd;
    int emissiveEnd;
    // int normalEnd;
    // int heightEnd;
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

uniform sampler2D textures[TEXTURE_CAPACITY];

uniform Material material;

uniform DirLight dirLight;
uniform PointLight pointLights[POINT_LIGHT_COUNT];
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
void SetValues(out vec3 ambient, out vec3 diffuse, out vec3 specular, out vec3 emissive);

float CalcSpec(vec3 fragToLight);

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
	vec3 fragToLight = normalize(-light.direction);

    float lambertian = max(dot(normal, fragToLight), 0.0);
    float spec = lambertian > 0.0 ? CalcSpec(fragToLight) : 0.0;

    vec3 ambient, diffuse, specular, emissive;

    SetValues(ambient, diffuse, specular, emissive);

    ambient  *= light.kA;
    diffuse  *= light.kD * lambertian;
    specular *= light.kS * spec;

    return (ambient + diffuse + specular + emissive) * light.color;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 fragToLight = normalize(light.position - fragPos);

    float lambertian = max(dot(normal, fragToLight), 0.0);
    float spec = lambertian > 0.0 ? CalcSpec(fragToLight) : 0.0;

    vec3 ambient, diffuse, specular, emissive;

    SetValues(ambient, diffuse, specular, emissive);

    ambient  *= light.kA * attenuation;
    diffuse  *= light.kD * attenuation * lambertian;
    specular *= light.kS * attenuation * spec;

    return (ambient + diffuse + specular + emissive) * light.color;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{

    vec3 fragToLight = normalize(light.position - fragPos);

    float phi = dot(fragToLight, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((phi - light.outerCutOff) / epsilon, 0.0, 1.0);

    float lambertian = max(dot(normal, fragToLight), 0.0);
    float spec = lambertian > 0.0 ? CalcSpec(fragToLight) : 0.0;
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient, diffuse, specular, emissive;

    SetValues(ambient, diffuse, specular, emissive);

    ambient  *= light.kA;
    diffuse  *= light.kD * intensity * attenuation * lambertian;
    specular *= light.kS * intensity * attenuation * spec;

    return (ambient + diffuse + specular + emissive) * light.color;
}

float CalcSpec(vec3 fragToLight)
{
    vec3 reflected = reflect(-fragToLight, normal);
    vec3 toViewer = normalize(viewPos - fragPos);

    float specAngle = max(dot(toViewer, reflected), 0.0);
    return pow(specAngle, material.shininess * 128.0);
}

void SetValues(out vec3 ambient, out vec3 diffuse, out vec3 specular, out vec3 emissive)
{
    // Iterate through diffuse textures
    for (int i = 0; i < material.diffuseEnd && i < TEXTURE_CAPACITY; i++)
    {
        ambient  += texture(textures[i], texCoord.st).rgb;
        diffuse  += texture(textures[i], texCoord.st).rgb;
    }

    // Iterate through specular textures
    for (int i = material.diffuseEnd; i < material.specularEnd && i < TEXTURE_CAPACITY; i++)
        specular += texture(textures[i], texCoord.st).rgb;

    // Iterate through emissive textures
    for (int i = material.specularEnd; i < material.emissiveEnd && i < TEXTURE_CAPACITY; i++)
        emissive += texture(textures[i], texCoord.st).rgb;
}
