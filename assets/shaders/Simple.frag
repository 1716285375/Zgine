#version 330 core

layout(location = 0) out vec4 FragColor;

in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoord;
in vec4 v_FragPosLightSpace;

// ---- Material ----
uniform vec3  u_Color;
uniform float u_Shininess; // specular exponent (higher = sharper highlights)

// ---- Camera ----
uniform vec3 u_CameraPos;

// ---- Shadow ----
uniform sampler2D u_ShadowMap;  // slot 5
uniform int u_EnableShadows;

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

// ---- Shadow calculation (3x3 PCF) ----
float CalcShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    if (u_EnableShadows == 0)
        return 0.0;

    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Outside shadow map => no shadow
    if (projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;

    // Bias based on surface angle to light
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

    // 3x3 PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

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

    // Directional light (with shadow)
    vec3 dirLightDir = normalize(-u_DirLight.direction);
    float shadow = CalcShadow(v_FragPosLightSpace, normal, dirLightDir);
    lighting += (1.0 - shadow) * CalcDirLight(u_DirLight, normal, viewDir);

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
