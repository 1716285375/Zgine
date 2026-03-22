#version 330 core

layout(location = 0) out vec4 FragColor;

in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoord;

// ---- Material ----
uniform vec3  u_Color;
uniform float u_Shininess; // specular exponent (higher = sharper highlights)

// ---- Camera ----
uniform vec3 u_CameraPos;

// ---- Directional Light ----
struct DirLight {
    vec3 direction;
    vec3 color; // pre-multiplied by intensity
};
uniform DirLight u_DirLight;

// ---- Point Lights ----
#define MAX_POINT_LIGHTS 8
struct PointLight {
    vec3  position;
    vec3  color;
    float constant;
    float linear;
    float quadratic;
};
uniform int        u_NumPointLights;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];

// ---- Spot Lights ----
#define MAX_SPOT_LIGHTS 8
struct SpotLight {
    vec3  position;
    vec3  direction;
    vec3  color;
    float cutOff;      // cos(inner cone angle)
    float outerCutOff; // cos(outer cone angle)
    float constant;
    float linear;
    float quadratic;
};
uniform int       u_NumSpotLights;
uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];

// ---- Blinn-Phong helpers ----

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular (Blinn-Phong)
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), u_Shininess);

    vec3 diffuse  = diff * light.color;
    vec3 specular = spec * light.color;

    return diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    // Attenuation
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular (Blinn-Phong)
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), u_Shininess);

    vec3 diffuse  = diff * light.color * attenuation;
    vec3 specular = spec * light.color * attenuation;

    return diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    // Spot intensity (smooth falloff between inner and outer cone)
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float spotIntensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Attenuation
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular (Blinn-Phong)
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), u_Shininess);

    vec3 diffuse  = diff * light.color * attenuation * spotIntensity;
    vec3 specular = spec * light.color * attenuation * spotIntensity;

    return diffuse + specular;
}

void main()
{
    vec3 normal  = normalize(v_Normal);
    vec3 viewDir = normalize(u_CameraPos - v_FragPos);

    // Ambient (fixed strength, not scaled by light intensity)
    vec3 ambient = vec3(0.1);

    // Accumulate light contributions
    vec3 lighting = vec3(0.0);

    // Directional light
    lighting += CalcDirLight(u_DirLight, normal, viewDir);

    // Point lights
    for (int i = 0; i < u_NumPointLights; i++)
        lighting += CalcPointLight(u_PointLights[i], normal, v_FragPos, viewDir);

    // Spot lights
    for (int i = 0; i < u_NumSpotLights; i++)
        lighting += CalcSpotLight(u_SpotLights[i], normal, v_FragPos, viewDir);

    vec3 result = (ambient + lighting) * u_Color;

    // Clamp before gamma (no HDR tone mapping yet)
    result = clamp(result, 0.0, 1.0);

    // Gamma correction
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, 1.0);
}
