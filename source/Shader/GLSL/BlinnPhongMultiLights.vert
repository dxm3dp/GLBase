layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;

out vec2 v_texCoords;
out vec3 v_tangentFragPos;
out vec3 v_tangentViewPos;
out mat3 v_world2tangent;

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projectionMat;
uniform vec3 u_viewPos;

void main()
{
    gl_Position = u_projectionMat * u_viewMat * u_modelMat * vec4(a_position, 1.0);
    v_texCoords = a_texCoords;

    vec3 T = normalize(mat3(u_modelMat) * a_tangent);
    vec3 N = normalize(mat3(u_modelMat) * a_normal);
    vec3 B = normalize(cross(N, T));
    mat3 transTBN = transpose(mat3(T, B, N));

    v_tangentFragPos = transTBN * vec3(u_modelMat * vec4(a_position, 1.0));
    v_tangentViewPos = transTBN * u_viewPos;
    v_world2tangent = transTBN;
}
