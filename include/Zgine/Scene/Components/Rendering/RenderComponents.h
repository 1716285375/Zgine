#pragma once

#include <Zgine/Core/Math/MathTypes.h>
#include <Zgine/Scene/Camera/Camera.h>
#include <Zgine/Resources/Mesh/PrimitiveMesh.h>
#include <memory>

namespace Zgine {

/**
 * @brief Camera component for rendering
 */
struct CameraComponent {
    std::shared_ptr<Camera> Camera;
    bool Primary = false;
    bool FixedAspectRatio = false;

    enum class ProjectionType : uint8_t {
        Perspective = 0,
        Orthographic = 1
    };

    CameraComponent() {
        Camera = std::make_shared<Zgine::Camera>();
    }
    CameraComponent(const CameraComponent&) = default;
};

/**
 * @brief Primitive shape component
 */
struct PrimitiveComponent {
    PrimitiveType Type = PrimitiveType::Cube;

    PrimitiveComponent() = default;
    PrimitiveComponent(const PrimitiveComponent&) = default;
    PrimitiveComponent(PrimitiveType type) : Type(type) {}
};

/**
 * @brief Sprite renderer component for 2D rendering
 */
struct SpriteRendererComponent {
    Math::Vector4 Color = Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    // TODO: Add texture support

    SpriteRendererComponent() = default;
    SpriteRendererComponent(const SpriteRendererComponent&) = default;
    SpriteRendererComponent(const Math::Vector4& color) : Color(color) {}
};

/**
 * @brief Color component for rendering
 */
struct ColorComponent {
    Math::Vector4 Color = Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    ColorComponent() = default;
    ColorComponent(const ColorComponent&) = default;
    ColorComponent(const Math::Vector4& color) : Color(color) {}
};

} // namespace Zgine
