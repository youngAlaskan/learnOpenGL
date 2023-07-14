#version 460

#define TEXTURE_CAPACITY 16

struct Material
{
    int diffuseEnd;
    int specularEnd;
    int emissiveEnd;
    // int normalEnd;
    // int heightEnd;
    float shininess;
};

uniform sampler2D textures[TEXTURE_CAPACITY];

uniform Material material;

void SetValues(out vec3 ambient, out vec3 diffuse, out vec3 specular, out vec3 emissive);

in vec3 normal;
in vec3 fragPos;
in vec3 color;
in vec3 texCoord;

out vec4 FragColor;

void main()
{
    vec3 ambient, diffuse, specular, emissive;

    SetValues(ambient, diffuse, specular, emissive);

    FragColor = vec4(ambient + diffuse + specular + emissive, 1.0);
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
