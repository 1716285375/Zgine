#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec3 u_Color;
uniform vec3 u_LightDirection;
uniform vec3 u_LightColor;

void main()
{
    // Simple diffuse lighting
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(-u_LightDirection);

    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * u_LightColor;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;

    vec3 result = (ambient + diffuse) * u_Color;
    color = vec4(result, 1.0);
}
