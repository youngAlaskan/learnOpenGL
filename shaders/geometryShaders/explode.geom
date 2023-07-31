#version 460
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} i_VertexData[];

out VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} o_VertexData;

uniform float time;

vec4 Explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
} 

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}  

void main()
{
    vec3 normal = GetNormal();

    gl_Position = Explode(gl_in[0].gl_Position, normal);
    o_VertexData.TexCoords = i_VertexData[0].TexCoords;
    EmitVertex();
    gl_Position = Explode(gl_in[1].gl_Position, normal);
    o_VertexData.TexCoords = i_VertexData[1].TexCoords;
    EmitVertex();
    gl_Position = Explode(gl_in[2].gl_Position, normal);
    o_VertexData.TexCoords = i_VertexData[2].TexCoords;
    EmitVertex();
    EndPrimitive();
}
