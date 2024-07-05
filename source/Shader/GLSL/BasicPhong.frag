layout(location = 0) in vec2 v_texCoords;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform vec3 lightColor;

void main()
{
    vec4 texColor = texture(texture_diffuse1, v_texCoords);
    FragColor = texColor * vec4(lightColor, 1.0);
}