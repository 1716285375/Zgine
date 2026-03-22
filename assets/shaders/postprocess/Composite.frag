#version 330 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_SceneTexture;
uniform sampler2D u_BloomTexture;
uniform int u_EnableBloom;
uniform float u_BloomStrength; // default 0.3

// ACES filmic tone mapping (fitted curve)
vec3 ACESFilm(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 hdrColor = texture(u_SceneTexture, v_TexCoord).rgb;

    // Add bloom
    if (u_EnableBloom == 1)
    {
        vec3 bloom = texture(u_BloomTexture, v_TexCoord).rgb;
        hdrColor += bloom * u_BloomStrength;
    }

    // ACES tone mapping
    vec3 mapped = ACESFilm(hdrColor);

    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}
