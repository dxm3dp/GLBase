in vec2 v_texCoords;
in vec3 v_tangentFragPos;
in vec3 v_tangentViewPos;
in mat3 v_world2tangent;

out vec4 FragColor;

struct Material
{
    vec3 ambient;
    float shininess;
};

struct DirLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material u_material;
uniform DirLight u_dirLight;
#define NR_POINT_LIGHTS 2
uniform PointLight u_pointLight[NR_POINT_LIGHTS];
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

vec3 calDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calPointLight(PointLight light, vec3 normal, vec3 viewDir);

void main()
{
    vec3 normal = texture(texture_normal1, v_texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec3 viewDir = normalize(v_tangentViewPos - v_tangentFragPos);

    vec3 result = calDirLight(u_dirLight, normal, viewDir);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        result += calPointLight(u_pointLight[i], normal, viewDir);
    }

    FragColor = vec4(result, 1.0);
}

vec3 calDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 ambient = light.ambient * u_material.ambient;

    vec3 lightDir = normalize(v_world2tangent * (-light.direction));
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuseColor = texture(texture_diffuse1, v_texCoords).rgb;
    vec3 diffuse = light.diffuse * diff * diffuseColor;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfwayDir, normal), 0.0), u_material.shininess);
    vec3 specularColor = texture(texture_specular1, v_texCoords).rgb;
    vec3 specular = light.specular * spec * specularColor;

    return (ambient + diffuse + specular);
}

vec3 calPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightPos = v_world2tangent * light.position;
    vec3 lightDir = normalize(lightPos - v_tangentFragPos);
    float distance = length(lightPos - v_tangentFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * u_material.ambient * attenuation;

    vec3 diffuseColor = texture(texture_diffuse1, v_texCoords).rgb;
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor * attenuation;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfwayDir, normal), 0.0), u_material.shininess);
    vec3 specularColor = texture(texture_specular1, v_texCoords).rgb;
    vec3 specular = light.specular * spec * specularColor * attenuation;

    return (ambient + diffuse + specular);
}
