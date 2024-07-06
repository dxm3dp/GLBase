layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec3 a_normal;

out vec2 v_texCoords;

uniform mat4 u_mvp;

void main()
{
    gl_Position = u_mvp * vec4(a_position, 1.0);
    v_texCoords = a_texCoords;
}