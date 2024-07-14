layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 FragPosLightSpace;
} vs_out;

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projectionMat;
uniform mat4 u_lightSpaceMat;

void main()
{
    vs_out.FragPos = vec3(u_modelMat * vec4(a_position, 1.0));
    vs_out.Normal = transpose(inverse(mat3(u_modelMat))) * a_normal;
    vs_out.TexCoords = a_texCoords;
    vs_out.FragPosLightSpace = vec3(u_lightSpaceMat * vec4(vs_out.FragPos, 1.0));
    gl_Position = u_projectionMat * u_viewMat * vec4(vs_out.FragPos, 1.0);
}