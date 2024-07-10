in vec2 v_texCoords;
in vec3 v_tangentFragPos;
in vec3 v_tangentLightPos;
in vec3 v_tangentViewPos;

out vec4 FragColor;

struct Material
{
    vec3 ambient;
    float shininess;
};

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform vec3 u_lightColor;
uniform Material u_material;

void main()
{
    // ambient
    vec3 ambient = u_lightColor * u_material.ambient;

    // diffuse
    vec3 normal = texture(texture_normal1, v_texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec3 lightDir = normalize(v_tangentLightPos - v_tangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuseColor = texture(texture_diffuse1, v_texCoords).rgb;
    vec3 diffuse = u_lightColor * diff * diffuseColor;

    // specular
    vec3 viewDir = normalize(v_tangentViewPos - v_tangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfwayDir, normal), 0.0), u_material.shininess);
    vec3 specularColor = texture(texture_specular1, v_texCoords).rgb;
    vec3 specular = u_lightColor * spec * specularColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}