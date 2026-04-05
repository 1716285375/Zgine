#pragma once

#include <Zgine/World/Serialization/IComponentSerializer.h>

namespace Zgine {

class ColorSerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "Color"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

class SpriteRendererSerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "SpriteRenderer"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

class MeshSerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "Mesh"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

} // namespace Zgine
