#pragma once

#include <Zgine/Core/Math/MathTypes.h>
#include <cstdint>

namespace Zgine {

/**
 * @brief Runtime handle for physics body (Jolt Physics)
 */
struct RuntimePhysicsBody {
    void* BodyPtr = nullptr;

    void Set(void* ptr) { BodyPtr = ptr; }
    void* Get() const { return BodyPtr; }
    bool IsValid() const { return BodyPtr != nullptr; }
    void Reset() { BodyPtr = nullptr; }
};

/**
 * @brief Rigidbody physics component type
 */
enum class RigidbodyType : uint8_t {
    Static = 0,
    Dynamic = 1,
    Kinematic = 2
};

/**
 * @brief Rigidbody component for physics simulation
 */
struct RigidbodyComponent {
    RigidbodyType Type = RigidbodyType::Dynamic;
    float Mass = 1.0f;
    float LinearDrag = 0.0f;
    float AngularDrag = 0.05f;
    float GravityScale = 1.0f;
    float Friction = 0.5f;
    float Restitution = 0.0f;
    bool FixedRotation = false;

    // Runtime physics body handle (don't serialize)
    RuntimePhysicsBody RuntimeBody;

    RigidbodyComponent() = default;
    RigidbodyComponent(const RigidbodyComponent&) = default;
};

/**
 * @brief Box collider component
 */
struct BoxColliderComponent {
    Math::Vector3 Size = Math::Vector3(1.0f, 1.0f, 1.0f);
    Math::Vector3 Offset = Math::Vector3(0.0f, 0.0f, 0.0f);
    bool IsTrigger = false;

    BoxColliderComponent() = default;
    BoxColliderComponent(const BoxColliderComponent&) = default;
};

/**
 * @brief Circle/Sphere collider component
 */
struct CircleColliderComponent {
    float Radius = 0.5f;
    Math::Vector3 Offset = Math::Vector3(0.0f, 0.0f, 0.0f);
    bool IsTrigger = false;

    CircleColliderComponent() = default;
    CircleColliderComponent(const CircleColliderComponent&) = default;
};

} // namespace Zgine
