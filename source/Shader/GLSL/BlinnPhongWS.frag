in vec2 v_texCoords;
in vec3 v_worldPosition;
in vec3 v_worldNormal;

out vec4 FragColor;

layout (binding = 1, std140) uniform UniformsScene
{
    vec3 u_ambientColor;
    vec3 u_cameraPosition;
    vec3 u_pointLightPosition;
    vec3 u_pointLightColor;
};

layout (binding = 2, std140) uniform UniformsMaterial
{
    float u_kSpecular;
    vec4 u_baseColor;
};

uniform sampler2D u_albedoMap;

void main()
{
    vec4 baseColor = texture(u_albedoMap, v_texCoords);

    // ambient
    vec3 ambient = baseColor.rgb * u_ambientColor;

    // diffuse
    vec3 normal = normalize(v_worldNormal);
    vec3 lightDir = normalize(u_pointLightPosition - v_worldPosition);
    float diffuse = max(dot(lightDir, normal), 0.0);
    vec3 diffuseColor = u_pointLightColor * diffuse * baseColor.rgb;

    // specular
    vec3 viewDir = normalize(u_cameraPosition - v_worldPosition);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular = pow(max(dot(halfwayDir, normal), 0.0), 64.0);
    vec3 specularColor = u_pointLightColor * specular * u_kSpecular;

    FragColor = vec4(ambient + diffuseColor + specularColor, 1.0);
}