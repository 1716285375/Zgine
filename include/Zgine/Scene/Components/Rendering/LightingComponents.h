#pragma once

#include <Zgine/Core/Math/MathTypes.h>
#include <string>
#include <memory>

namespace Zgine {

/**
 * @brief Directional light component (e.g., sun light)
 */
struct DirectionalLightComponent {
    Math::Vector3 Direction = Math::Vector3(0.0f, -1.0f, 0.0f);
    Math::Vector3 Color = Math::Vector3(1.0f, 1.0f, 1.0f);
    float Intensity = 1.0f;
    bool CastShadows = true;

    DirectionalLightComponent() = default;
    DirectionalLightComponent(const DirectionalLightComponent&) = default;
};

/**
 * @brief Point light component (omni-directional)
 */
struct PointLightComponent {
    Math::Vector3 Position = Math::Vector3(0.0f, 0.0f, 0.0f);
    Math::Vector3 Color = Math::Vector3(1.0f, 1.0f, 1.0f);
    float Intensity = 1.0f;
    float Radius = 10.0f;  // Alternative name for Range
    float Range = 10.0f;
    float Constant = 1.0f;
    float Linear = 0.09f;
    float Quadratic = 0.032f;
    bool CastShadows = false;

    PointLightComponent() = default;
    PointLightComponent(const PointLightComponent&) = default;
};

/**
 * @brief Spot light component (cone-shaped)
 */
struct SpotLightComponent {
    Math::Vector3 Position = Math::Vector3(0.0f, 0.0f, 0.0f);
    Math::Vector3 Direction = Math::Vector3(0.0f, -1.0f, 0.0f);
    Math::Vector3 Color = Math::Vector3(1.0f, 1.0f, 1.0f);
    float Intensity = 1.0f;
    float Range = 10.0f;
    float InnerCone = 12.5f;  // Original name
    float OuterCone = 17.5f;  // Original name
    float InnerCutoff = 12.5f;  // Alternative name for inspector
    float OuterCutoff = 17.5f;  // Alternative name for inspector
    bool CastShadows = false;

    SpotLightComponent() = default;
    SpotLightComponent(const SpotLightComponent&) = default;
};

} // namespace Zgine
