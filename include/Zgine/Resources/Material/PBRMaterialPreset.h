#pragma once

#include <Zgine/Core/Math/MathTypes.h>
#include <string>
#include <unordered_map>

namespace Zgine {

/**
 * @brief PBR Material Preset definition
 */
struct PBRMaterialPreset {
    std::string Name;
    Math::Vector3 Albedo;
    float Metallic;
    float Roughness;
    float AO;

    PBRMaterialPreset() = default;
    PBRMaterialPreset(const std::string& name, const Math::Vector3& albedo, float metallic, float roughness, float ao = 1.0f)
        : Name(name), Albedo(albedo), Metallic(metallic), Roughness(roughness), AO(ao) {}
};

/**
 * @brief Registry for predefined PBR material presets
 */
class PBRMaterialPresetRegistry {
public:
    static void Initialize();
    static const PBRMaterialPreset* GetPreset(const std::string& name);
    static const std::unordered_map<std::string, PBRMaterialPreset>& GetAllPresets();

private:
    static void RegisterPreset(const PBRMaterialPreset& preset);
    static std::unordered_map<std::string, PBRMaterialPreset> s_Presets;
};

} // namespace Zgine
