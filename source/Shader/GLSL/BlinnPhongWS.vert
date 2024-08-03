layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;

out vec2 v_texCoords;
out vec3 v_worldPos;
out vec3 v_worldNormal;
out vec3 v_worldLightDir;
out vec3 v_worldViewDir;
out vec4 v_shadowFragPos;

#if defined(NORMAL_MAP)
out vec3 v_worldTangent;
#endif

layout(binding = 0, std140) uniform UniformsModel
{
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

void main()
{
    vec4 position = vec4(a_position, 1.0);
    gl_Position = u_modelViewProjectionMatrix * position;
    v_texCoords = a_texCoords;
    v_shadowFragPos = u_shadowMVPMatrix * position;

    v_worldPos = vec3(u_modelMatrix * position);
    v_worldNormal = mat3(u_modelMatrix) * a_normal;
    v_worldLightDir = u_pointLightPosition - v_worldPos;
    v_worldViewDir = u_cameraPosition - v_worldPos;

#if defined(NORMAL_MAP)
    vec3 N = normalize(mat3(u_inverseTransposeModelMatrix) * a_normal);
    vec3 T = normalize(mat3(u_inverseTransposeModelMatrix) * a_tangent);
    v_worldNormal = N;
    v_worldTangent = normalize(T - dot(T, N) * N);
#endif
}