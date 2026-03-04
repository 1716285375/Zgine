#include <Zgine/Resources/Material/PBRMaterialPreset.h>

namespace Zgine {

std::unordered_map<std::string, PBRMaterialPreset> PBRMaterialPresetRegistry::s_Presets;

void PBRMaterialPresetRegistry::Initialize() {
    using namespace Math;

    // Metals
    RegisterPreset(PBRMaterialPreset("Gold", Vector3(1.0f, 0.765557f, 0.336057f), 1.0f, 0.2f));
    RegisterPreset(PBRMaterialPreset("Copper", Vector3(0.955f, 0.637f, 0.538f), 1.0f, 0.3f));
    RegisterPreset(PBRMaterialPreset("Chrome", Vector3(0.55f, 0.55f, 0.55f), 1.0f, 0.1f));
    RegisterPreset(PBRMaterialPreset("Iron", Vector3(0.56f, 0.57f, 0.58f), 1.0f, 0.4f));
    RegisterPreset(PBRMaterialPreset("Silver", Vector3(0.972f, 0.960f, 0.915f), 1.0f, 0.2f));

    // Plastics
    RegisterPreset(PBRMaterialPreset("Plastic Red", Vector3(0.8f, 0.1f, 0.1f), 0.0f, 0.5f));
    RegisterPreset(PBRMaterialPreset("Plastic Blue", Vector3(0.1f, 0.3f, 0.8f), 0.0f, 0.5f));
    RegisterPreset(PBRMaterialPreset("Plastic Green", Vector3(0.1f, 0.8f, 0.1f), 0.0f, 0.5f));
    RegisterPreset(PBRMaterialPreset("Rubber", Vector3(0.2f, 0.2f, 0.2f), 0.0f, 0.9f));
    RegisterPreset(PBRMaterialPreset("Rubber Black", Vector3(0.05f, 0.05f, 0.05f), 0.0f, 0.95f));
    RegisterPreset(PBRMaterialPreset("Ceramic", Vector3(0.9f, 0.9f, 0.9f), 0.0f, 0.1f));

    // Natural materials
    RegisterPreset(PBRMaterialPreset("Rough Concrete", Vector3(0.5f, 0.5f, 0.5f), 0.0f, 0.9f));
    RegisterPreset(PBRMaterialPreset("Smooth Concrete", Vector3(0.5f, 0.5f, 0.5f), 0.0f, 0.3f));
    RegisterPreset(PBRMaterialPreset("Wood", Vector3(0.6f, 0.4f, 0.2f), 0.0f, 0.7f));
    RegisterPreset(PBRMaterialPreset("Glass", Vector3(0.9f, 0.9f, 0.9f), 0.0f, 0.05f));
}

const PBRMaterialPreset* PBRMaterialPresetRegistry::GetPreset(const std::string& name) {
    auto it = s_Presets.find(name);
    if (it != s_Presets.end()) {
        return &it->second;
    }
    return nullptr;
}

const std::unordered_map<std::string, PBRMaterialPreset>& PBRMaterialPresetRegistry::GetAllPresets() {
    return s_Presets;
}

void PBRMaterialPresetRegistry::RegisterPreset(const PBRMaterialPreset& preset) {
    s_Presets[preset.Name] = preset;
}

} // namespace Zgine
