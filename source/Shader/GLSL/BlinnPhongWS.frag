in vec2 v_texCoords;
in vec3 v_worldPos;
in vec3 v_worldNormal;
in vec3 v_worldLightDir;
in vec3 v_worldViewDir;
in vec4 v_shadowFragPos;

#if defined(NORMAL_MAP)
in vec3 v_worldTangent;
#endif

out vec4 FragColor;

layout (binding = 0, std140) uniform UniformsModel {
    mat4 u_modelMatrix;
    mat4 u_modelViewProjectionMatrix;
    mat3 u_inverseTransposeModelMatrix;
    mat4 u_shadowMVPMatrix;
};

layout(binding = 1, std140) uniform UniformsScene
{
    vec3 u_ambientColor;
    vec3 u_cameraPosition;
    vec3 u_pointLightPosition;
    vec3 u_pointLightColor;
};

layout(binding = 2, std140) uniform UniformsMaterial
{
    float u_kSpecular;
    vec4 u_baseColor;
};

#if defined(ALBEDO_MAP)
uniform sampler2D u_albedoMap;
#endif

#if defined(NORMAL_MAP)
uniform sampler2D u_normalMap;
#endif

#if defined(EMISSIVE_MAP)
uniform sampler2D u_emissiveMap;
#endif

#if defined(AO_MAP)
uniform sampler2D u_aoMap;
#endif

uniform sampler2D u_shadowMap;

const float depthBiasCoeff = 0.00025;
const float depthBiasMin = 0.00005;

vec3 GetNormal()
{
#if defined(NORMAL_MAP)
    vec3 N = normalize(v_worldNormal);
    vec3 T = normalize(v_worldTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(T, N);
    mat3 TBN = mat3(T, B, N);

    vec3 tangentNormal = texture(u_normalMap, v_texCoords).rgb;
    tangentNormal = tangentNormal * 2.0 - 1.0;
    return normalize(TBN * tangentNormal);
#else
    return normalize(v_worldNormal);
#endif
}

float ShadowCalculation(vec4 fragPos, vec3 normal)
{
    vec3 projCoords = fragPos.xyz / fragPos.w;
    float currentDepth = projCoords.z;

#if defined(OpenGL)// [-1, 1] -> [0, 1]
    currentDepth = currentDepth * 0.5 + 0.5;
#endif

    if (currentDepth < 0.0 || currentDepth > 1.0)
    {
        return 0.0;
    }

    float bias = max(depthBiasCoeff * (1.0 - dot(normal, normalize(v_worldLightDir))), depthBiasMin);
#if defined(OpenGL)
    bias = bias * 0.5;
#endif

    float shadow = 0.0;
    vec2 pixelOffset = 1.0 / textureSize(u_shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * pixelOffset).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

void main()
{
#if defined(ALBEDO_MAP)
    vec4 baseColor = texture(u_albedoMap, v_texCoords);
#else
    vec4 baseColor = u_baseColor;
#endif

    vec3 N = GetNormal();

    // ambient
    float ao = 1.0;
#if defined(AO_MAP)
    ao = texture(u_aoMap, v_texCoords).r;
#endif
    vec3 ambient = baseColor.rgb * u_ambientColor * ao;

    // diffuse
    vec3 lightDir = normalize(v_worldLightDir);
    float diff = max(dot(lightDir, N), 0.0);
    vec3 diffuse = u_pointLightColor * diff * baseColor.rgb;

    // specular
    vec3 viewDir = normalize(v_worldViewDir);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(N, halfwayDir), 0.0), 128.0);
    vec3 specular = u_pointLightColor * u_kSpecular * spec;

    // calculate shadow
    float shadow = 1.0 - ShadowCalculation(v_shadowFragPos, N);
    diffuse *= shadow;
    specular *= shadow;

    vec3 emissive = vec3(0.0);
#if defined(EMISSIVE_MAP)
    emissive = texture(u_emissiveMap, v_texCoords).rgb;
#endif

    FragColor = vec4(ambient + diffuse + specular + emissive, baseColor.a);
}