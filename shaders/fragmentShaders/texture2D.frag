#version 460

#define TEXTURE_CAPACITY 16
#define BASE_COLOR_MASK 1
#define ALBEDO_MASK     2
#define METALLIC_MASK   4
#define ROUGHNESS_MASK  8
#define AMBIENT_MASK    16
#define NORMAL_MASK     32
#define HEIGHT_MASK     64
#define EMISSIONS_MASK  128

struct Material
{
    int activeMaps;
    float shininess;
};

uniform sampler2D textures[TEXTURE_CAPACITY];

uniform Material material;

in VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} i_VertexData;

out vec4 FragColor;

void main()
{
    if (bool(material.activeMaps & BASE_COLOR_MASK)) {
	    FragColor = vec4(texture(textures[0], i_VertexData.TexCoords));
    } else {
        FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    }
}