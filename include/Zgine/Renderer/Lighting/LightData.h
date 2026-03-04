#pragma once

#include <Zgine/Core/Math/MathTypes.h>

namespace Zgine {

/**
 * @brief Lighting data for rendering
 */
struct DirectionalLightData {
    Math::Vector3 direction;
    Math::Vector3 color;
    float intensity;
};

struct PointLightData {
    Math::Vector3 position;
    Math::Vector3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLightData {
    Math::Vector3 position;
    Math::Vector3 direction;
    Math::Vector3 color;
    float intensity;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

struct LightingData {
    DirectionalLightData directional;
    PointLightData points[8];
    SpotLightData spots[8];
    int numPointLights = 0;
    int numSpotLights = 0;
};

} // namespace Zgine
