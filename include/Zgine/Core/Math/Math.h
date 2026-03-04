#pragma once

namespace Zgine::Math {

// Forward declarations
struct Vector3;
struct Vector4;
struct Matrix4;
struct Quaternion;

// Common constants
constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 6.28318530717958647692f;
constexpr float HALF_PI = 1.57079632679489661923f;
constexpr float EPSILON = 1e-6f;
constexpr float DEG2RAD = PI / 180.0f;
constexpr float RAD2DEG = 180.0f / PI;

// Scalar math utilities
float Clamp(float value, float min, float max);
float Lerp(float a, float b, float t);
float Smoothstep(float edge0, float edge1, float x);
bool Approximately(float a, float b, float epsilon = EPSILON);
float Sign(float value);
float DegToRad(float degrees);
float RadToDeg(float radians);
float Abs(float value);
float Min(float a, float b);
float Max(float a, float b);
float Sqrt(float value);
float Pow(float base, float exponent);
float Sin(float angle);
float Cos(float angle);
float Tan(float angle);
float Asin(float value);
float Acos(float value);
float Atan(float value);
float Atan2(float y, float x);

} // namespace Zgine::Math
