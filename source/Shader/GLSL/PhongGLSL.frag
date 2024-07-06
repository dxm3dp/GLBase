in vec2 v_texCoords;
in vec3 v_worldPos;
in vec3 v_normal;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec4 texColor = texture(texture_diffuse1, v_texCoords);

    // ambient
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 lightDir = normalize(lightPos - v_worldPos);
    vec3 normal = normalize(v_normal);
    vec3 diffuse = max(dot(lightDir, normal), 0.0) * lightColor;

    // specular
    float specualrStrength = 0.5;
    vec3 viewDir = normalize(viewPos - v_worldPos);
    vec3 reflectDir = normalize(reflect(-lightDir, viewDir));
    vec3 specualr = pow(max(dot(reflectDir, viewDir), 0.0), 32.0) * specualrStrength * lightColor;

    FragColor = vec4(ambient + diffuse + specualr, 1.0) * texColor;
}