#version 450 core

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 4
#define MAX_CASCADES 4

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float range;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;
    float innerCone;
    float outerCone;
};

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoords;
in mat3 v_TBN;

out vec4 FragColor;

// 材质纹理
uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_MetallicTexture;
uniform sampler2D u_RoughnessTexture;
uniform sampler2D u_AOTexture;

// 材质参数
uniform vec3 u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AO;

// 纹理使用标志
uniform bool u_UseAlbedoTexture;
uniform bool u_UseNormalTexture;
uniform bool u_UseMetallicTexture;
uniform bool u_UseRoughnessTexture;
uniform bool u_UseAOTexture;

// 光照
uniform DirectionalLight u_DirectionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform int u_DirectionalLightCount;

uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_PointLightCount;

uniform SpotLight u_SpotLights[MAX_SPOT_LIGHTS];
uniform int u_SpotLightCount;

// 相机
uniform vec3 u_CameraPosition;

// 环境贴图
uniform samplerCube u_IrradianceMap;
uniform samplerCube u_PrefilterMap;
uniform sampler2D u_BRDFLUT;
uniform bool u_UseIBL;

// 阴影
uniform sampler2D u_ShadowMaps[MAX_CASCADES];
uniform mat4 u_LightSpaceMatrices[MAX_CASCADES];
uniform float u_CascadeFarPlanes[MAX_CASCADES];
uniform int u_CascadeCount;
uniform bool u_EnableShadows;

const float PI = 3.14159265359;

float CalculateShadow(vec3 fragPos, vec3 normal);

// Cook-Torrance BRDF 函数
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// 计算 PBR 光照
vec3 CalculatePBR(vec3 albedo, float metallic, float roughness, float ao, vec3 N, vec3 V, vec3 F0)
{
    vec3 Lo = vec3(0.0);

    // 方向光
    for (int i = 0; i < u_DirectionalLightCount; i++) {
        vec3 L = normalize(-u_DirectionalLights[i].direction);
        vec3 H = normalize(V + L);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            // Cook-Torrance BRDF
            float NDF = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;

            vec3 numerator = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
            vec3 specular = numerator / denominator;

            Lo += (kD * albedo / PI + specular) * u_DirectionalLights[i].color * u_DirectionalLights[i].intensity * NdotL;
        }
    }

    // 点光源
    for (int i = 0; i < u_PointLightCount; i++) {
        vec3 L = normalize(u_PointLights[i].position - v_FragPos);
        vec3 H = normalize(V + L);
        float distance = length(u_PointLights[i].position - v_FragPos);
        float attenuation = 1.0 / (u_PointLights[i].constant + u_PointLights[i].linear * distance +
                                   u_PointLights[i].quadratic * (distance * distance));
        attenuation = smoothstep(u_PointLights[i].range, 0.0, distance) * attenuation;

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            float NDF = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;

            vec3 numerator = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
            vec3 specular = numerator / denominator;

            Lo += (kD * albedo / PI + specular) * u_PointLights[i].color * u_PointLights[i].intensity * NdotL * attenuation;
        }
    }

    // 聚光灯
    for (int i = 0; i < u_SpotLightCount; i++) {
        vec3 L = normalize(u_SpotLights[i].position - v_FragPos);
        vec3 H = normalize(V + L);
        float distance = length(u_SpotLights[i].position - v_FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
        attenuation = smoothstep(u_SpotLights[i].range, 0.0, distance) * attenuation;

        float theta = dot(L, normalize(-u_SpotLights[i].direction));
        float epsilon = u_SpotLights[i].innerCone - u_SpotLights[i].outerCone;
        float intensity = clamp((theta - u_SpotLights[i].outerCone) / epsilon, 0.0, 1.0);
        attenuation *= intensity;

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            float NDF = DistributionGGX(N, H, roughness);
            float G = GeometrySmith(N, V, L, roughness);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;

            vec3 numerator = NDF * G * F;
            float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
            vec3 specular = numerator / denominator;

            Lo += (kD * albedo / PI + specular) * u_SpotLights[i].color * u_SpotLights[i].intensity * NdotL * attenuation;
        }
    }

    // IBL（图像基础照明）
    // When IBL is off, use a higher ambient to make objects visible
    vec3 ambient = vec3(0.15) * albedo * ao;  // Increased from 0.03 for better visibility without IBL
    if (u_UseIBL) {
        vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;

        vec3 irradiance = texture(u_IrradianceMap, N).rgb;
        vec3 diffuse = irradiance * albedo;

        vec3 R = reflect(-V, N);
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(u_PrefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
        vec2 brdf = texture(u_BRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

        ambient = (kD * diffuse + specular) * ao;
    }

    vec3 color = ambient + Lo;

    // 阴影计算
    float shadow = 1.0;
    if (u_EnableShadows && u_CascadeCount > 0) {
        shadow = CalculateShadow(v_FragPos, N);
    }

    color = color * shadow;

    // HDR 色调映射（Reinhard）
    color = color / (color + vec3(1.0));

    // Gamma 校正
    color = pow(color, vec3(1.0 / 2.2));

    return color;
}

// PCF 软阴影计算
float CalculateShadow(vec3 fragPos, vec3 normal) {
    if (u_CascadeCount == 0) {
        return 1.0;
    }

    // 确定使用哪个级联
    float viewZ = length(fragPos - u_CameraPosition);
    int cascadeIndex = 0;
    for (int i = 0; i < u_CascadeCount - 1; i++) {
        if (viewZ > u_CascadeFarPlanes[i]) {
            cascadeIndex = i + 1;
        }
    }

    // 转换到光照空间
    vec4 fragPosLightSpace = u_LightSpaceMatrices[cascadeIndex] * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // 转换到 [0,1] 范围

    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0) {
        return 1.0; // 在阴影贴图范围外
    }

    // PCF 软阴影
    float currentDepth = projCoords.z;

    // \u4f7f\u7528\u56fa\u5b9a\u504f\u79fb\uff0c\u907f\u514d\u8bbf\u95ee\u53ef\u80fd\u4e0d\u5b58\u5728\u7684\u5149\u6e90\n    // Use fixed bias to avoid accessing potentially non-existent light source
    float bias = 0.005;

    float shadowValue = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMaps[cascadeIndex], 0);

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            float pcfDepth = texture(u_ShadowMaps[cascadeIndex], projCoords.xy + vec2(x, y) * texelSize).r;
            shadowValue += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadowValue /= 9.0;

    return 1.0 - shadowValue;
}

void main()
{
    // 采样材质纹理
    vec3 albedo = u_UseAlbedoTexture ? texture(u_AlbedoTexture, v_TexCoords).rgb : u_Albedo;
    float metallic = u_UseMetallicTexture ? texture(u_MetallicTexture, v_TexCoords).r : u_Metallic;
    float roughness = u_UseRoughnessTexture ? texture(u_RoughnessTexture, v_TexCoords).r : u_Roughness;
    float ao = u_UseAOTexture ? texture(u_AOTexture, v_TexCoords).r : u_AO;

    // 法线贴图
    vec3 N = v_Normal;
    if (u_UseNormalTexture) {
        N = texture(u_NormalTexture, v_TexCoords).rgb;
        N = N * 2.0 - 1.0;
        N = normalize(v_TBN * N);
    }

    vec3 V = normalize(u_CameraPosition - v_FragPos);

    // 计算 F0（基础反射率）
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 color = CalculatePBR(albedo, metallic, roughness, ao, N, V, F0);

    FragColor = vec4(color, 1.0);
}
