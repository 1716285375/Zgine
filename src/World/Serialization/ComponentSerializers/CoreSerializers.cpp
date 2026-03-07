#include <Zgine/World/Serialization/ComponentSerializers/CoreSerializers.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Core/Math/MathTypes.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Zgine {

// ============================================================================
// TransformSerializer
// ============================================================================

void TransformSerializer::Serialize(const Entity& entity, json& out) const {
    auto& transform = const_cast<Entity&>(entity).GetComponent<TransformComponent>();
    out["Transform"] = json::object();
    out["Transform"]["Translation"] = {
        transform.Translation.x,
        transform.Translation.y,
        transform.Translation.z
    };
    out["Transform"]["Rotation"] = {
        transform.Rotation.x,
        transform.Rotation.y,
        transform.Rotation.z
    };
    out["Transform"]["Scale"] = {
        transform.Scale.x,
        transform.Scale.y,
        transform.Scale.z
    };
}

bool TransformSerializer::Deserialize(const json& data, Entity& entity) const {
    auto& transform = entity.GetComponent<TransformComponent>();

    if (data.contains("Translation")) {
        const auto& trans = data["Translation"];
         transform.Translation = Math::Vector3(
            trans[0].get<float>(),
            trans[1].get<float>(),
            trans[2].get<float>()
        );
    }
    if (data.contains("Rotation")) {
        const auto& rot = data["Rotation"];
        transform.Rotation = Math::Vector3(
            rot[0].get<float>(),
            rot[1].get<float>(),
            rot[2].get<float>()
        );
    }
    if (data.contains("Scale")) {
        const auto& scale = data["Scale"];
        transform.Scale = Math::Vector3(
            scale[0].get<float>(),
            scale[1].get<float>(),
            scale[2].get<float>()
        );
    }
    return true;
}

bool TransformSerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<TransformComponent>();
}

// ============================================================================
// CameraSerializer
// ============================================================================

void CameraSerializer::Serialize(const Entity& entity, json& out) const {
    auto& camera = const_cast<Entity&>(entity).GetComponent<CameraComponent>();
    out["Camera"] = json::object();
    out["Camera"]["Primary"] = camera.Primary;
}

bool CameraSerializer::Deserialize(const json& data, Entity& entity) const {
    // Create camera with default projection (will be adjusted at runtime)
    auto& camera = entity.AddComponent<CameraComponent>();

    if (data.contains("Primary")) {
        camera.Primary = data["Primary"].get<bool>();
    }
    return true;
}

bool CameraSerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<CameraComponent>();
}

// ============================================================================
// PrimitiveSerializer
// ============================================================================

void PrimitiveSerializer::Serialize(const Entity& entity, json& out) const {
    auto& primitive = const_cast<Entity&>(entity).GetComponent<PrimitiveComponent>();
    out["Primitive"] = json::object();
    out["Primitive"]["Type"] = static_cast<int>(primitive.Type);
}

bool PrimitiveSerializer::Deserialize(const json& data, Entity& entity) const {
    if (data.contains("Type")) {
        entity.AddComponent<PrimitiveComponent>(
            static_cast<PrimitiveType>(data["Type"].get<int>()));
    }
    return true;
}

bool PrimitiveSerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<PrimitiveComponent>();
}

// ============================================================================
// PBRMaterialSerializer
// ============================================================================

void PBRMaterialSerializer::Serialize(const Entity& entity, json& out) const {
    auto& material = const_cast<Entity&>(entity).GetComponent<PBRMaterialComponent>();
    out["PBRMaterial"] = json::object();
    auto& mat = out["PBRMaterial"];

    mat["Albedo"] = { material.Albedo.x, material.Albedo.y, material.Albedo.z };
    mat["Metallic"] = material.Metallic;
    mat["Roughness"] = material.Roughness;
    mat["AO"] = material.AO;

    mat["UseAlbedoTexture"] = material.UseAlbedoTexture;
    mat["UseNormalTexture"] = material.UseNormalTexture;
    mat["UseMetallicTexture"] = material.UseMetallicTexture;
    mat["UseRoughnessTexture"] = material.UseRoughnessTexture;
    mat["UseAOTexture"] = material.UseAOTexture;

    mat["AlbedoTexturePath"] = material.AlbedoTexturePath;
    mat["NormalTexturePath"] = material.NormalTexturePath;
    mat["MetallicTexturePath"] = material.MetallicTexturePath;
    mat["RoughnessTexturePath"] = material.RoughnessTexturePath;
    mat["AOTexturePath"] = material.AOTexturePath;

    if (material.AlbedoTextureHandle.IsValid())
        mat["AlbedoHandle"] = material.AlbedoTextureHandle.ToString();
    if (material.NormalTextureHandle.IsValid())
        mat["NormalHandle"] = material.NormalTextureHandle.ToString();
    if (material.MetallicTextureHandle.IsValid())
        mat["MetallicHandle"] = material.MetallicTextureHandle.ToString();
    if (material.RoughnessTextureHandle.IsValid())
        mat["RoughnessHandle"] = material.RoughnessTextureHandle.ToString();
    if (material.AOTextureHandle.IsValid())
        mat["AOHandle"] = material.AOTextureHandle.ToString();
}

bool PBRMaterialSerializer::Deserialize(const json& data, Entity& entity) const {
    auto& material = entity.AddComponent<PBRMaterialComponent>();

    if (data.contains("Albedo")) {
        const auto& albedo = data["Albedo"];
        material.Albedo = Math::Vector3(
            albedo[0].get<float>(),
            albedo[1].get<float>(),
            albedo[2].get<float>()
        );
    }

    if (data.contains("Metallic")) material.Metallic = data["Metallic"].get<float>();
    if (data.contains("Roughness")) material.Roughness = data["Roughness"].get<float>();
    if (data.contains("AO")) material.AO = data["AO"].get<float>();

    if (data.contains("UseAlbedoTexture")) material.UseAlbedoTexture = data["UseAlbedoTexture"].get<bool>();
    if (data.contains("UseNormalTexture")) material.UseNormalTexture = data["UseNormalTexture"].get<bool>();
    if (data.contains("UseMetallicTexture")) material.UseMetallicTexture = data["UseMetallicTexture"].get<bool>();
    if (data.contains("UseRoughnessTexture")) material.UseRoughnessTexture = data["UseRoughnessTexture"].get<bool>();
    if (data.contains("UseAOTexture")) material.UseAOTexture = data["UseAOTexture"].get<bool>();

    if (data.contains("AlbedoTexturePath")) material.AlbedoTexturePath = data["AlbedoTexturePath"].get<std::string>();
    if (data.contains("NormalTexturePath")) material.NormalTexturePath = data["NormalTexturePath"].get<std::string>();
    if (data.contains("MetallicTexturePath")) material.MetallicTexturePath = data["MetallicTexturePath"].get<std::string>();
    if (data.contains("RoughnessTexturePath")) material.RoughnessTexturePath = data["RoughnessTexturePath"].get<std::string>();
    if (data.contains("AOTexturePath")) material.AOTexturePath = data["AOTexturePath"].get<std::string>();

    if (data.contains("AlbedoHandle")) material.AlbedoTextureHandle = AssetHandle::FromString(data["AlbedoHandle"].get<std::string>());
    if (data.contains("NormalHandle")) material.NormalTextureHandle = AssetHandle::FromString(data["NormalHandle"].get<std::string>());
    if (data.contains("MetallicHandle")) material.MetallicTextureHandle = AssetHandle::FromString(data["MetallicHandle"].get<std::string>());
    if (data.contains("RoughnessHandle")) material.RoughnessTextureHandle = AssetHandle::FromString(data["RoughnessHandle"].get<std::string>());
    if (data.contains("AOHandle")) material.AOTextureHandle = AssetHandle::FromString(data["AOHandle"].get<std::string>());

    return true;
}

bool PBRMaterialSerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<PBRMaterialComponent>();
}

// ============================================================================
// Light Serializers
// ============================================================================

void LightSerializers::DirectionalLight::Serialize(const Entity& entity, json& out) const {
    auto& light = const_cast<Entity&>(entity).GetComponent<DirectionalLightComponent>();
    out["DirectionalLight"] = json::object();
    auto& l = out["DirectionalLight"];

    l["Direction"] = { light.Direction.x, light.Direction.y, light.Direction.z };
    l["Color"] = { light.Color.x, light.Color.y, light.Color.z };
    l["Intensity"] = light.Intensity;
    l["CastShadows"] = light.CastShadows;
}

bool LightSerializers::DirectionalLight::Deserialize(const json& data, Entity& entity) const {
    auto& light = entity.AddComponent<DirectionalLightComponent>();

    if (data.contains("Direction")) {
        const auto& dir = data["Direction"];
        light.Direction = Math::Vector3(dir[0].get<float>(), dir[1].get<float>(), dir[2].get<float>());
    }
    if (data.contains("Color")) {
        const auto& color = data["Color"];
        light.Color = Math::Vector3(color[0].get<float>(), color[1].get<float>(), color[2].get<float>());
    }
    if (data.contains("Intensity")) light.Intensity = data["Intensity"].get<float>();
    if (data.contains("CastShadows")) light.CastShadows = data["CastShadows"].get<bool>();

    return true;
}

bool LightSerializers::DirectionalLight::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<DirectionalLightComponent>();
}

void LightSerializers::PointLight::Serialize(const Entity& entity, json& out) const {
    auto& light = const_cast<Entity&>(entity).GetComponent<PointLightComponent>();
    out["PointLight"] = json::object();
    auto& l = out["PointLight"];

    l["Color"] = { light.Color.x, light.Color.y, light.Color.z };
    l["Intensity"] = light.Intensity;
    l["Range"] = light.Range;
    l["Constant"] = light.Constant;
    l["Linear"] = light.Linear;
    l["Quadratic"] = light.Quadratic;
    l["CastShadows"] = light.CastShadows;
}

bool LightSerializers::PointLight::Deserialize(const json& data, Entity& entity) const {
    auto& light = entity.AddComponent<PointLightComponent>();

    if (data.contains("Color")) {
        const auto& color = data["Color"];
        light.Color = Math::Vector3(color[0].get<float>(), color[1].get<float>(), color[2].get<float>());
    }
    if (data.contains("Intensity")) light.Intensity = data["Intensity"].get<float>();
    if (data.contains("Range")) light.Range = data["Range"].get<float>();
    if (data.contains("Constant")) light.Constant = data["Constant"].get<float>();
    if (data.contains("Linear")) light.Linear = data["Linear"].get<float>();
    if (data.contains("Quadratic")) light.Quadratic = data["Quadratic"].get<float>();
    if (data.contains("CastShadows")) light.CastShadows = data["CastShadows"].get<bool>();

    return true;
}

bool LightSerializers::PointLight::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<PointLightComponent>();
}

void LightSerializers::SpotLight::Serialize(const Entity& entity, json& out) const {
    auto& light = const_cast<Entity&>(entity).GetComponent<SpotLightComponent>();
    out["SpotLight"] = json::object();
    auto& l = out["SpotLight"];

    l["Direction"] = { light.Direction.x, light.Direction.y, light.Direction.z };
    l["Color"] = { light.Color.x, light.Color.y, light.Color.z };
    l["Intensity"] = light.Intensity;
    l["Range"] = light.Range;
    l["InnerCone"] = light.InnerCone;
    l["OuterCone"] = light.OuterCone;
    l["CastShadows"] = light.CastShadows;
}

bool LightSerializers::SpotLight::Deserialize(const json& data, Entity& entity) const {
    auto& light = entity.AddComponent<SpotLightComponent>();

    if (data.contains("Direction")) {
        const auto& dir = data["Direction"];
        light.Direction = Math::Vector3(dir[0].get<float>(), dir[1].get<float>(), dir[2].get<float>());
    }
    if (data.contains("Color")) {
        const auto& color = data["Color"];
        light.Color = Math::Vector3(color[0].get<float>(), color[1].get<float>(), color[2].get<float>());
    }
    if (data.contains("Intensity")) light.Intensity = data["Intensity"].get<float>();
    if (data.contains("Range")) light.Range = data["Range"].get<float>();
    if (data.contains("InnerCone")) light.InnerCone = data["InnerCone"].get<float>();
    if (data.contains("OuterCone")) light.OuterCone = data["OuterCone"].get<float>();
    if (data.contains("CastShadows")) light.CastShadows = data["CastShadows"].get<bool>();

    return true;
}

bool LightSerializers::SpotLight::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<SpotLightComponent>();
}

} // namespace Zgine
