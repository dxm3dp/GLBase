in vec2 v_texCoord;

out vec4 FragColor;

layout(binding = 1, std140) uniform UniformsMaterial
{
    vec4 u_baseColor;
    float u_kSpecular;
};

uniform sampler2D u_albedoMap;

void main()
{
    vec4 baseColor = texture(u_albedoMap, v_texCoord);
    FragColor = baseColor;
}