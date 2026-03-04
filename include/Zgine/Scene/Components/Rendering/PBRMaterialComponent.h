#pragma once

#include <Zgine/Core/Math/MathTypes.h>
#include <Zgine/Resources/Core/AssetHandle.h>
#include <string>
#include <memory>

namespace Zgine {

class Texture;

/**
 * @brief PBR (Physically Based Rendering) material component
 */
struct PBRMaterialComponent {
    // Texture paths
    std::string AlbedoTexturePath;
    std::string NormalTexturePath;
    std::string MetallicTexturePath;
    std::string RoughnessTexturePath;
    std::string AOTexturePath;

    // Asset handles (for persistence)
    AssetHandle AlbedoTextureHandle;
    AssetHandle NormalTextureHandle;
    AssetHandle MetallicTextureHandle;
    AssetHandle RoughnessTextureHandle;
    AssetHandle AOTextureHandle;

    // Runtime texture objects
    std::shared_ptr<Texture> AlbedoTexture;
    std::shared_ptr<Texture> NormalTexture;
    std::shared_ptr<Texture> MetallicTexture;
    std::shared_ptr<Texture> RoughnessTexture;
    std::shared_ptr<Texture> AOTexture;

    // Material parameters (fallback values)
    Math::Vector3 Albedo = Math::Vector3(0.8f, 0.8f, 0.8f);
    float Metallic = 0.0f;
    float Roughness = 0.5f;
    float AO = 1.0f;

    // Texture usage flags
    bool UseAlbedoTexture = false;
    bool UseNormalTexture = false;
    bool UseMetallicTexture = false;
    bool UseRoughnessTexture = false;
    bool UseAOTexture = false;

    PBRMaterialComponent() = default;
    PBRMaterialComponent(const PBRMaterialComponent&) = default;
};

} // namespace Zgine
