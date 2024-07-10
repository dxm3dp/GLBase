layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;

out vec2 v_texCoords;
out vec3 v_tangentFragPos;
out vec3 v_tangentLightPos;
out vec3 v_tangentViewPos;

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projectionMat;
uniform vec3 u_lightPos;
uniform vec3 u_viewPos;

void main()
{
    gl_Position = u_projectionMat * u_viewMat * u_modelMat * vec4(a_position, 1.0);
    v_texCoords = a_texCoords;

    vec3 T = normalize(vec3(u_modelMat * vec4(a_tangent, 0)));
    vec3 N = normalize(vec3(u_modelMat * vec4(a_normal, 0)));
    vec3 B = normalize(cross(N, T));
    mat3 transTBN = transpose(mat3(T, B, N));

    v_tangentFragPos = transTBN * vec3(u_modelMat * vec4(a_position, 1.0));
    v_tangentLightPos = transTBN * u_lightPos;
    v_tangentViewPos = transTBN * u_viewPos;
}
