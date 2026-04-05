#include <Zgine/World/Serialization/ComponentSerializers/RenderingSerializers.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Zgine {

// ============================================================================
// ColorSerializer
// ============================================================================

void ColorSerializer::Serialize(const Entity& entity, json& out) const {
    auto& color = const_cast<Entity&>(entity).GetComponent<ColorComponent>();
    out["Color"] = json::object();
    out["Color"]["Color"] = { color.Color.x, color.Color.y, color.Color.z, color.Color.w };
}

bool ColorSerializer::Deserialize(const json& data, Entity& entity) const {
    auto& color = entity.AddComponent<ColorComponent>();

    if (data.contains("Color")) {
        const auto& c = data["Color"];
        color.Color = Math::Vector4(c[0].get<float>(), c[1].get<float>(), c[2].get<float>(), c[3].get<float>());
    }
    return true;
}

bool ColorSerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<ColorComponent>();
}

// ============================================================================
// SpriteRendererSerializer
// ============================================================================

void SpriteRendererSerializer::Serialize(const Entity& entity, json& out) const {
    auto& sprite = const_cast<Entity&>(entity).GetComponent<SpriteRendererComponent>();
    out["SpriteRenderer"] = json::object();
    out["SpriteRenderer"]["Color"] = { sprite.Color.x, sprite.Color.y, sprite.Color.z, sprite.Color.w };
}

bool SpriteRendererSerializer::Deserialize(const json& data, Entity& entity) const {
    auto& sprite = entity.AddComponent<SpriteRendererComponent>();

    if (data.contains("Color")) {
        const auto& c = data["Color"];
        sprite.Color = Math::Vector4(c[0].get<float>(), c[1].get<float>(), c[2].get<float>(), c[3].get<float>());
    }
    return true;
}

bool SpriteRendererSerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<SpriteRendererComponent>();
}

// ============================================================================
// MeshSerializer
// ============================================================================

void MeshSerializer::Serialize(const Entity& entity, json& out) const {
    auto& mesh = const_cast<Entity&>(entity).GetComponent<MeshComponent>();
    out["Mesh"] = json::object();

    if (mesh.MeshHandle.IsValid())
        out["Mesh"]["MeshHandle"] = mesh.MeshHandle.ToString();
}

bool MeshSerializer::Deserialize(const json& data, Entity& entity) const {
    auto& mesh = entity.AddComponent<MeshComponent>();

    if (data.contains("MeshHandle"))
        mesh.MeshHandle = AssetHandle::FromString(data["MeshHandle"].get<std::string>());

    return true;
}

bool MeshSerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<MeshComponent>();
}

} // namespace Zgine
