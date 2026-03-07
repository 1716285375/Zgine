#pragma once

#include <Zgine/Core/Math/MathTypes.h>

namespace Zgine {

/**
 * @brief Transform component for entity position, rotation, and scale
 */
struct TransformComponent {
    Math::Vector3 Translation = { 0.0f, 0.0f, 0.0f };
    Math::Vector3 Rotation = { 0.0f, 0.0f, 0.0f };
    Math::Vector3 Scale = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const Math::Vector3& translation) : Translation(translation) {}

    Math::Matrix4 GetTransform() const {
        using namespace Math;

        Matrix4 rotation = Matrix4::Rotation(Rotation.x, Vector3(1, 0, 0))
                         * Matrix4::Rotation(Rotation.y, Vector3(0, 1, 0))
                         * Matrix4::Rotation(Rotation.z, Vector3(0, 0, 1));

        return Matrix4::Translation(Translation)
             * rotation
             * Matrix4::Scale(Scale);
    }
};

} // namespace Zgine
