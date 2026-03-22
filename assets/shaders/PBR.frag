#version 330 core

layout(location = 0) out vec4 FragColor;

in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoord;
in vec4 v_FragPosLightSpace;

// ---- Constants ----
const float PI = 3.14159265359;

// ---- Material textures ----
uniform sampler2D u_AlbedoMap;     // slot 0
uniform sampler2D u_NormalMap;     // slot 1
uniform sampler2D u_MetallicMap;   // slot 2
uniform sampler2D u_RoughnessMap;  // slot 3
uniform sampler2D u_AOMap;         // slot 4

// ---- Material scalars (used when texture is disabled) ----
uniform vec3  u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AO;

// ---- Texture usage flags ----
uniform int u_UseAlbedoMap;
uniform int u_UseNormalMap;
uniform int u_UseMetallicMap;
uniform int u_UseRoughnessMap;
uniform int u_UseAOMap;

// ---- Camera ----
uniform vec3 u_CameraPos;

// ---- Shadow ----
uniform sampler2D u_ShadowMap;  // slot 5
uniform int u_EnableShadows;

// ---- Lights (same as Blinn-Phong shader) ----
struct DirLight {
    vec3 direction;
    vec3 color;
};
uniform DirLight u_DirLight;

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

#define MAX_SPOT_LIGHTS 8
struct SpotLight {
    vec3  position;
    vec3  direction;
    vec3  color;
    float cutOff;
    float outerCutOff;
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

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

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

// ---- PBR functions ----

// Normal Distribution Function (GGX/Trowbridge-Reitz)
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a  = roughness * roughness;
    float a2 = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    return a2 / (PI * denom * denom);
}

// Geometry function (Schlick-GGX)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

// Smith's method
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

// Fresnel (Schlick approximation)
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ---- Normal mapping via screen-space derivatives ----
vec3 GetNormal()
{
    vec3 N = normalize(v_Normal);

    if (u_UseNormalMap == 0)
        return N;

    // Sample tangent-space normal from map
    vec3 tangentNormal = texture(u_NormalMap, v_TexCoord).rgb * 2.0 - 1.0;

    // Compute TBN from screen-space derivatives
    vec3 Q1  = dFdx(v_FragPos);
    vec3 Q2  = dFdy(v_FragPos);
    vec2 st1 = dFdx(v_TexCoord);
    vec2 st2 = dFdy(v_TexCoord);

    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// ---- Cook-Torrance BRDF for a single light ----
vec3 CalcPBRLight(vec3 lightDir, vec3 radiance, vec3 N, vec3 V,
                  vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 H = normalize(V + lightDir);

    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, lightDir, roughness);
    vec3  F   = FresnelSchlick(max(dot(H, V), 0.0), F0);

    // Specular
    vec3  numerator   = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, lightDir), 0.0) + 0.0001;
    vec3  specular    = numerator / denominator;

    // Energy conservation: diffuse = 1 - specular (metals have no diffuse)
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);

    float NdotL = max(dot(N, lightDir), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

void main()
{
    // Sample material properties
    vec3  albedo    = u_UseAlbedoMap    == 1 ? pow(texture(u_AlbedoMap, v_TexCoord).rgb, vec3(2.2)) : u_Albedo;
    float metallic  = u_UseMetallicMap  == 1 ? texture(u_MetallicMap, v_TexCoord).r : u_Metallic;
    float roughness = u_UseRoughnessMap == 1 ? texture(u_RoughnessMap, v_TexCoord).r : u_Roughness;
    float ao        = u_UseAOMap        == 1 ? texture(u_AOMap, v_TexCoord).r : u_AO;

    // Clamp roughness to avoid divide-by-zero in GGX
    roughness = clamp(roughness, 0.04, 1.0);

    vec3 N = GetNormal();
    vec3 V = normalize(u_CameraPos - v_FragPos);

    // F0: reflectance at normal incidence (0.04 for dielectrics, albedo for metals)
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // Accumulate lighting
    vec3 Lo = vec3(0.0);

    // Directional light (with shadow)
    {
        vec3 L = normalize(-u_DirLight.direction);
        float shadow = CalcShadow(v_FragPosLightSpace, N, L);
        Lo += (1.0 - shadow) * CalcPBRLight(L, u_DirLight.color, N, V, albedo, metallic, roughness, F0);
    }

    // Point lights
    for (int i = 0; i < u_NumPointLights; i++)
    {
        vec3 L = normalize(u_PointLights[i].position - v_FragPos);
        float dist = length(u_PointLights[i].position - v_FragPos);
        float attenuation = 1.0 / (u_PointLights[i].constant + u_PointLights[i].linear * dist
                          + u_PointLights[i].quadratic * dist * dist);
        vec3 radiance = u_PointLights[i].color * attenuation;
        Lo += CalcPBRLight(L, radiance, N, V, albedo, metallic, roughness, F0);
    }

    // Spot lights
    for (int i = 0; i < u_NumSpotLights; i++)
    {
        vec3 L = normalize(u_SpotLights[i].position - v_FragPos);
        float dist = length(u_SpotLights[i].position - v_FragPos);
        float attenuation = 1.0 / (u_SpotLights[i].constant + u_SpotLights[i].linear * dist
                          + u_SpotLights[i].quadratic * dist * dist);

        float theta   = dot(L, normalize(-u_SpotLights[i].direction));
        float epsilon = u_SpotLights[i].cutOff - u_SpotLights[i].outerCutOff;
        float spotIntensity = clamp((theta - u_SpotLights[i].outerCutOff) / epsilon, 0.0, 1.0);

        vec3 radiance = u_SpotLights[i].color * attenuation * spotIntensity;
        Lo += CalcPBRLight(L, radiance, N, V, albedo, metallic, roughness, F0);
    }

    // Ambient (simple constant, replaced by IBL later)
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // Clamp (no HDR tone mapping yet)
    color = clamp(color, 0.0, 1.0);

    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
