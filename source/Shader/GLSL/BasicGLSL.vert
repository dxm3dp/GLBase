layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;

out vec2 v_texCoord;

layout(binding = 0, std140) uniform UniformsModel
{
    mat4 u_modelMatrix;
    mat4 u_modelViewProjectionMatrix;
    mat3 u_inverseTransposeModelMatrix;
};

layout(binding = 1, std140) uniform UniformsMaterial
{
    vec4 u_baseColor;
    float u_kSpecular;
};

void main()
{
    gl_Position = u_modelViewProjectionMatrix * vec4(a_position, 1.0);
    v_texCoord = a_texCoord;
}