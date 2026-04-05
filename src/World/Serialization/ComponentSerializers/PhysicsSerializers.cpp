#include <Zgine/World/Serialization/ComponentSerializers/PhysicsSerializers.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Components/Components.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Zgine {

// ============================================================================
// RigidbodySerializer
// ============================================================================

void RigidbodySerializer::Serialize(const Entity& entity, json& out) const {
    auto& rb = const_cast<Entity&>(entity).GetComponent<RigidbodyComponent>();
    out["Rigidbody"] = json::object();
    auto& j = out["Rigidbody"];

    j["Type"] = static_cast<int>(rb.Type);
    j["Mass"] = rb.Mass;
    j["LinearDrag"] = rb.LinearDrag;
    j["AngularDrag"] = rb.AngularDrag;
    j["GravityScale"] = rb.GravityScale;
    j["Friction"] = rb.Friction;
    j["Restitution"] = rb.Restitution;
    j["FixedRotation"] = rb.FixedRotation;
}

bool RigidbodySerializer::Deserialize(const json& data, Entity& entity) const {
    auto& rb = entity.AddComponent<RigidbodyComponent>();

    if (data.contains("Type")) rb.Type = static_cast<RigidbodyType>(data["Type"].get<int>());
    if (data.contains("Mass")) rb.Mass = data["Mass"].get<float>();
    if (data.contains("LinearDrag")) rb.LinearDrag = data["LinearDrag"].get<float>();
    if (data.contains("AngularDrag")) rb.AngularDrag = data["AngularDrag"].get<float>();
    if (data.contains("GravityScale")) rb.GravityScale = data["GravityScale"].get<float>();
    if (data.contains("Friction")) rb.Friction = data["Friction"].get<float>();
    if (data.contains("Restitution")) rb.Restitution = data["Restitution"].get<float>();
    if (data.contains("FixedRotation")) rb.FixedRotation = data["FixedRotation"].get<bool>();

    return true;
}

bool RigidbodySerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<RigidbodyComponent>();
}

// ============================================================================
// BoxColliderSerializer
// ============================================================================

void BoxColliderSerializer::Serialize(const Entity& entity, json& out) const {
    auto& collider = const_cast<Entity&>(entity).GetComponent<BoxColliderComponent>();
    out["BoxCollider"] = json::object();
    auto& j = out["BoxCollider"];

    j["Size"] = { collider.Size.x, collider.Size.y, collider.Size.z };
    j["Offset"] = { collider.Offset.x, collider.Offset.y, collider.Offset.z };
    j["IsTrigger"] = collider.IsTrigger;
}

bool BoxColliderSerializer::Deserialize(const json& data, Entity& entity) const {
    auto& collider = entity.AddComponent<BoxColliderComponent>();

    if (data.contains("Size")) {
        const auto& s = data["Size"];
        collider.Size = Math::Vector3(s[0].get<float>(), s[1].get<float>(), s[2].get<float>());
    }
    if (data.contains("Offset")) {
        const auto& o = data["Offset"];
        collider.Offset = Math::Vector3(o[0].get<float>(), o[1].get<float>(), o[2].get<float>());
    }
    if (data.contains("IsTrigger")) collider.IsTrigger = data["IsTrigger"].get<bool>();

    return true;
}

bool BoxColliderSerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<BoxColliderComponent>();
}

// ============================================================================
// CircleColliderSerializer
// ============================================================================

void CircleColliderSerializer::Serialize(const Entity& entity, json& out) const {
    auto& collider = const_cast<Entity&>(entity).GetComponent<CircleColliderComponent>();
    out["CircleCollider"] = json::object();
    auto& j = out["CircleCollider"];

    j["Radius"] = collider.Radius;
    j["Offset"] = { collider.Offset.x, collider.Offset.y, collider.Offset.z };
    j["IsTrigger"] = collider.IsTrigger;
}

bool CircleColliderSerializer::Deserialize(const json& data, Entity& entity) const {
    auto& collider = entity.AddComponent<CircleColliderComponent>();

    if (data.contains("Radius")) collider.Radius = data["Radius"].get<float>();
    if (data.contains("Offset")) {
        const auto& o = data["Offset"];
        collider.Offset = Math::Vector3(o[0].get<float>(), o[1].get<float>(), o[2].get<float>());
    }
    if (data.contains("IsTrigger")) collider.IsTrigger = data["IsTrigger"].get<bool>();

    return true;
}

bool CircleColliderSerializer::HasComponent(const Entity& entity) const {
    return const_cast<Entity&>(entity).HasComponent<CircleColliderComponent>();
}

} // namespace Zgine
