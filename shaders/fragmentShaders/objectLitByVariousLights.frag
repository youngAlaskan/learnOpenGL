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
    vec4 color;

	float kA;
    float kD;
    float kS;
};

struct PointLight
{
    vec4 position;
    vec4 color;

    float kA;
    float kD;
    float kS;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec4 position;
    vec3 direction;
    vec4 color;

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

vec4 CalcDirLight(in DirLight light, in vec3 normal, in vec3 toViewer);
vec4 CalcPointLight(in PointLight light, in vec3 normal, in vec3 toViewer);
vec4 CalcSpotLight(in SpotLight light, in vec3 normal, in vec3 toViewer);
void SetValues(out vec4 ambient, out vec4 diffuse, out vec4 specular, out vec4 emissive);

float CalcSpec(in vec3 fragToLight, in vec3 toViewer);

uniform vec3 viewPos;

in vec4 fragPos;
in vec4 color;
in vec3 normal;
in vec2 texCoord;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(normal);
    vec3 toViewer = normalize(viewPos - vec3(fragPos));

    vec4 result = vec4(0.0);
    if (dirLight.direction != vec3(0.0))
        result += CalcDirLight(dirLight, norm, toViewer);

    for (int i = 0; i < POINT_LIGHT_COUNT; i++)
    {
        result += CalcPointLight(pointLights[i], norm, toViewer);
    }

    if (spotLight.innerCutOff != 0.0)
        result += CalcSpotLight(spotLight, norm, toViewer);

    FragColor = result;
}

vec4 CalcDirLight(DirLight light, vec3 normal, in vec3 toViewer)
{
	vec3 fragToLight = normalize(-light.direction);

    float lambertian = max(dot(normal, fragToLight), 0.0);
    float spec = lambertian > 0.0 ? CalcSpec(fragToLight, toViewer) : 0.0;

    vec4 ambient, diffuse, specular, emissive;

    SetValues(ambient, diffuse, specular, emissive);

    ambient  *= light.kA;
    diffuse  *= light.kD * lambertian;
    specular *= light.kS * spec;

    return (ambient + diffuse + specular + emissive) * light.color;
}

vec4 CalcPointLight(PointLight light, vec3 normal, in vec3 toViewer)
{
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 fragToLight = normalize(vec3(light.position - fragPos));

    float lambertian = max(dot(normal, fragToLight), 0.0);
    float spec = lambertian > 0.0 ? CalcSpec(fragToLight, toViewer) : 0.0;

    vec4 ambient, diffuse, specular, emissive;

    SetValues(ambient, diffuse, specular, emissive);

    ambient  *= light.kA * attenuation;
    diffuse  *= light.kD * attenuation * lambertian;
    specular *= light.kS * attenuation * spec;

    return (ambient + diffuse + specular + emissive) * light.color;
}

vec4 CalcSpotLight(SpotLight light, vec3 normal, in vec3 toViewer)
{

    vec3 fragToLight = normalize(vec3(light.position - fragPos));

    float phi = dot(fragToLight, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((phi - light.outerCutOff) / epsilon, 0.0, 1.0);

    float lambertian = max(dot(normal, fragToLight), 0.0);
    float spec = lambertian > 0.0 ? CalcSpec(fragToLight, toViewer) : 0.0;
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec4 ambient, diffuse, specular, emissive;

    SetValues(ambient, diffuse, specular, emissive);

    ambient  *= light.kA;
    diffuse  *= light.kD * intensity * attenuation * lambertian;
    specular *= light.kS * intensity * attenuation * spec;

    return (ambient + diffuse + specular + emissive) * light.color;
}

float CalcSpec(vec3 fragToLight, vec3 toViewer)
{
    vec3 reflected = reflect(-fragToLight, normal);

    float specAngle = max(dot(toViewer, reflected), 0.0);
    return pow(specAngle, material.shininess * 128.0);
}

void SetValues(out vec4 ambient, out vec4 diffuse, out vec4 specular, out vec4 emissive)
{
    // Iterate through diffuse textures
    for (int i = 0; i < material.diffuseEnd && i < TEXTURE_CAPACITY; i++)
    {
        if (texture(textures[i], texCoord).a == 0.0)
            discard;
        ambient  += texture(textures[i], texCoord);
        diffuse  += texture(textures[i], texCoord);
    }

    // Iterate through specular textures
    for (int i = material.diffuseEnd; i < material.specularEnd && i < TEXTURE_CAPACITY; i++)
        specular += texture(textures[i], texCoord);

    // Iterate through emissive textures
    for (int i = material.specularEnd; i < material.emissiveEnd && i < TEXTURE_CAPACITY; i++)
        emissive += texture(textures[i], texCoord);
}
