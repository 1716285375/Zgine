#pragma once

#include <Zgine/World/Serialization/IComponentSerializer.h>

namespace Zgine {

class RigidbodySerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "Rigidbody"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

class BoxColliderSerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "BoxCollider"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

class CircleColliderSerializer : public IComponentSerializer {
public:
    std::string_view GetComponentTypeName() const override { return "CircleCollider"; }
    void Serialize(const Entity& entity, nlohmann::json& out) const override;
    bool Deserialize(const nlohmann::json& data, Entity& entity) const override;
    bool HasComponent(const Entity& entity) const override;
};

} // namespace Zgine
