#version 460 core

#define TEXTURE_CAPACITY 16
#define POINT_LIGHT_CAPACITY 1

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
uniform samplerCube skybox;

uniform Material material;

uniform int pointLightCount;

uniform DirLight dirLight;
uniform PointLight pointLights[POINT_LIGHT_CAPACITY];
uniform SpotLight spotLight;

uniform vec3 viewPos;

in VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} i_VertexData;

out vec4 FragColor;

vec4 CalcDirLight(in DirLight light, in vec3 toViewer, in mat4 textureValues);
vec4 CalcPointLight(in PointLight light, in vec3 toViewer, in mat4 textureValues);
vec4 CalcSpotLight(in SpotLight light, in vec3 toViewer, in mat4 textureValues);
void SetValues(out mat4 textureValues);

float CalcSpec(in vec3 fragToLight, in vec3 toViewer);

void main()
{

    vec3 toViewer = normalize(viewPos - vec3(i_VertexData.FragPos));

    mat4 textureValues; // Ambient, Diffuse, Specular, Emissive

    SetValues(textureValues);

    vec4 result = vec4(0.0);
    if (dirLight.direction != vec3(0.0))
        result += CalcDirLight(dirLight, toViewer, textureValues);

    for (int i = 0; i < pointLightCount && i < POINT_LIGHT_CAPACITY; i++)
        result += CalcPointLight(pointLights[i], toViewer, textureValues);

    if (spotLight.innerCutOff != 0.0)
        result += CalcSpotLight(spotLight, toViewer, textureValues);

    FragColor = vec4(result.rgb, textureValues[0].a);
}

vec4 CalcDirLight(DirLight light, in vec3 toViewer, in mat4 textureValues)
{
	vec3 fragToLight = normalize(-light.direction);

    float lambertian = max(dot(i_VertexData.Normal, fragToLight), 0.0);
    float spec = lambertian > 0.0 ? CalcSpec(fragToLight, toViewer) : 0.0;

    vec4 ambient  = textureValues[0];
    vec4 diffuse  = textureValues[1];
    vec4 specular = textureValues[2];
    vec4 emissive = textureValues[3];

    ambient  *= light.kA;
    diffuse  *= light.kD * lambertian;
    specular *= light.kS * spec;

    return (ambient + diffuse + specular + emissive) * light.color;
}

vec4 CalcPointLight(PointLight light, in vec3 toViewer, in mat4 textureValues)
{
    float distance = length(light.position - i_VertexData.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 fragToLight = normalize(vec3(light.position - i_VertexData.FragPos));

    float lambertian = max(dot(i_VertexData.Normal, fragToLight), 0.0);
    float spec = lambertian > 0.0 ? CalcSpec(fragToLight, toViewer) : 0.0;

    vec4 ambient  = textureValues[0];
    vec4 diffuse  = textureValues[1];
    vec4 specular = textureValues[2];
    vec4 emissive = textureValues[3];

    ambient  *= light.kA * attenuation;
    diffuse  *= light.kD * attenuation * lambertian;
    specular *= light.kS * attenuation * spec;

    return (ambient + diffuse + specular + emissive) * light.color;
}

vec4 CalcSpotLight(SpotLight light, in vec3 toViewer, in mat4 textureValues)
{

    vec3 fragToLight = normalize(vec3(light.position - i_VertexData.FragPos));

    float phi = dot(fragToLight, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((phi - light.outerCutOff) / epsilon, 0.0, 1.0);

    float lambertian = max(dot(i_VertexData.Normal, fragToLight), 0.0);
    float spec = lambertian > 0.0 ? CalcSpec(fragToLight, toViewer) : 0.0;
    
    float distance = length(light.position - i_VertexData.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec4 ambient  = textureValues[0];
    vec4 diffuse  = textureValues[1];
    vec4 specular = textureValues[2];
    vec4 emissive = textureValues[3];

    ambient  *= light.kA;
    diffuse  *= light.kD * intensity * attenuation * lambertian;
    specular *= light.kS * intensity * attenuation * spec;

    return (ambient + diffuse + specular + emissive) * light.color;
}

float CalcSpec(in vec3 fragToLight, in vec3 toViewer)
{
    vec3 reflected = reflect(-fragToLight, i_VertexData.Normal);

    float specAngle = max(dot(toViewer, reflected), 0.0);
    return pow(specAngle, material.shininess * 128.0);
}

void SetValues(out mat4 textureValues)
{
    // Iterate through diffuse textures
    for (int i = 0; i < material.diffuseEnd && i < TEXTURE_CAPACITY; i++)
    {
        if (texture(textures[i], i_VertexData.TexCoords).a == 0.0)
            discard;
        textureValues[0] += texture(textures[i], i_VertexData.TexCoords);
        textureValues[1] += texture(textures[i], i_VertexData.TexCoords);
    }

    // Iterate through specular textures
    for (int i = material.diffuseEnd; i < material.specularEnd && i < TEXTURE_CAPACITY; i++)
        textureValues[2] += texture(textures[i], i_VertexData.TexCoords);

    // Iterate through emissive textures
    for (int i = material.specularEnd; i < material.emissiveEnd && i < TEXTURE_CAPACITY; i++)
        textureValues[3] += texture(textures[i], i_VertexData.TexCoords);
}
