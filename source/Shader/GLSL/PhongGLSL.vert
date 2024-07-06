layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;

out vec2 v_texCoords;
out vec3 v_worldPos;
out vec3 v_normal;

uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat3 u_inversTransModel;

void main()
{
    gl_Position = u_mvp * vec4(a_position, 1.0);
    v_texCoords = a_texCoords;
    v_worldPos = vec3(u_model * vec4(a_position, 1.0));
    v_normal = u_inversTransModel * a_normal;
}