in vec2 v_texCoords;
in vec3 v_worldPos;
in vec3 v_normal;

out vec4 FragColor;

struct Material
{
    vec3 ambient;
    float shininess;
};

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;

void main()
{
    // ambient
    vec3 ambient = lightColor * material.ambient;

    // diffuse
    vec3 lightDir = normalize(lightPos - v_worldPos);
    vec3 normal = normalize(v_normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec4 diffuseColor = texture(texture_diffuse1, v_texCoords);
    vec3 diffuse = lightColor * diff * diffuseColor.rgb;

    // specular
    vec3 viewDir = normalize(viewPos - v_worldPos);
    vec3 reflectDir = normalize(reflect(-lightDir, viewDir));
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec4 specularColor = texture(texture_specular1, v_texCoords);
    vec3 specular = lightColor * spec * specularColor.rgb;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}