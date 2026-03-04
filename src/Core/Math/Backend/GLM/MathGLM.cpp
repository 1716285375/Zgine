// GLM Backend Implementation for Math utilities
#include <Zgine/Core/Math/Math.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

namespace Zgine::Math {

float Clamp(float value, float min, float max) {
    return glm::clamp(value, min, max);
}

float Lerp(float a, float b, float t) {
    return glm::mix(a, b, t);
}

float Smoothstep(float edge0, float edge1, float x) {
    return glm::smoothstep(edge0, edge1, x);
}

bool Approximately(float a, float b, float epsilon) {
    return glm::abs(a - b) < epsilon;
}

float Sign(float value) {
    return value < 0.0f ? -1.0f : 1.0f;
}

float DegToRad(float degrees) {
    return glm::radians(degrees);
}

float RadToDeg(float radians) {
    return glm::degrees(radians);
}

float Abs(float value) {
    return glm::abs(value);
}

float Min(float a, float b) {
    return glm::min(a, b);
}

float Max(float a, float b) {
    return glm::max(a, b);
}

float Sqrt(float value) {
    return glm::sqrt(value);
}

float Pow(float base, float exponent) {
    return glm::pow(base, exponent);
}

float Sin(float angle) {
    return glm::sin(angle);
}

float Cos(float angle) {
    return glm::cos(angle);
}

float Tan(float angle) {
    return glm::tan(angle);
}

float Asin(float value) {
    return glm::asin(value);
}

float Acos(float value) {
    return glm::acos(value);
}

float Atan(float value) {
    return glm::atan(value);
}

float Atan2(float y, float x) {
    return glm::atan(y, x);
}

} // namespace Zgine::Math
