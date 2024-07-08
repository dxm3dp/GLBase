in vec3 v_tangentFragPos;
in vec2 v_texCoords;
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
uniform vec3 lightColor;
uniform Material material;

void main()
{
    // ambient
    vec3 ambient = lightColor * material.ambient;

    // diffuse
    vec3 normal = texture(texture_normal1, v_texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec3 lightDir = normalize(v_tangentLightPos - v_tangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec4 diffuseColor = texture(texture_diffuse1, v_texCoords);
    vec3 diffuse = lightColor * diff * diffuseColor.rgb;

    // specular
    vec3 viewDir = normalize(v_tangentViewPos - v_tangentFragPos);
    vec3 reflectDir = normalize(reflect(-lightDir, viewDir));
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec4 specularColor = texture(texture_specular1, v_texCoords);
    vec3 specular = lightColor * spec * specularColor.rgb;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}