layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;

out vec2 v_texCoords;
out vec3 v_worldPosition;
out vec3 v_worldNormal;

layout(binding = 0, std140) uniform UniformsModel
{
    mat4 u_modelMatrix;
    mat4 u_modelViewProjectionMatrix;
    mat3 u_inverseTransposeModelMatrix;
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
    gl_Position = u_modelViewProjectionMatrix * vec4(a_position, 1.0);
    v_texCoords = a_texCoords;

    v_worldPosition = vec3(u_modelMatrix * vec4(a_position, 1.0));
    v_worldNormal = mat3(transpose(inverse(u_modelMatrix))) * a_normal;
    //v_worldNormal = u_inverseTransposeModelMatrix * a_normal;
}