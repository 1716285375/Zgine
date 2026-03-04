// GLM Backend Implementation for Vector2
#include <Zgine/Core/Math/Vector2.h>
#include <Zgine/Core/Math/Math.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace Zgine::Math {

// Static members
const Vector2 Vector2::Zero(0.0f, 0.0f);
const Vector2 Vector2::One(1.0f, 1.0f);
const Vector2 Vector2::UnitX(1.0f, 0.0f);
const Vector2 Vector2::UnitY(0.0f, 1.0f);

// Constructors
Vector2::Vector2() : x(0.0f), y(0.0f) {}
Vector2::Vector2(float scalar) : x(scalar), y(scalar) {}
Vector2::Vector2(float x, float y) : x(x), y(y) {}

// Operators
Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(float scalar) const {
    return Vector2(x / scalar, y / scalar);
}

Vector2& Vector2::operator+=(const Vector2& other) {
    x += other.x; y += other.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& other) {
    x -= other.x; y -= other.y;
    return *this;
}

Vector2& Vector2::operator*=(float scalar) {
    x *= scalar; y *= scalar;
    return *this;
}

Vector2& Vector2::operator/=(float scalar) {
    x /= scalar; y /= scalar;
    return *this;
}

Vector2 Vector2::operator-() const {
    return Vector2(-x, -y);
}

// Comparison operators
bool Vector2::operator==(const Vector2& other) const {
    return glm::abs(x - other.x) < EPSILON &&
           glm::abs(y - other.y) < EPSILON;
}

auto Vector2::operator<=>(const Vector2& other) const {
    if (auto cmp = x <=> other.x; cmp != 0) return cmp;
    return y <=> other.y;
}

float& Vector2::operator[](int index) {
    return (&x)[index];
}

const float& Vector2::operator[](int index) const {
    return (&x)[index];
}

// Vector operations using GLM
float Length(const Vector2& v) {
    return glm::length(glm::vec2(v.x, v.y));
}

float LengthSquared(const Vector2& v) {
    return glm::length2(glm::vec2(v.x, v.y));
}

Vector2 Normalize(const Vector2& v) {
    glm::vec2 result = glm::normalize(glm::vec2(v.x, v.y));
    return Vector2(result.x, result.y);
}

float Distance(const Vector2& a, const Vector2& b) {
    return glm::distance(glm::vec2(a.x, a.y), glm::vec2(b.x, b.y));
}

float Dot(const Vector2& a, const Vector2& b) {
    return glm::dot(glm::vec2(a.x, a.y), glm::vec2(b.x, b.y));
}

Vector2 Lerp(const Vector2& a, const Vector2& b, float t) {
    glm::vec2 result = glm::mix(glm::vec2(a.x, a.y), glm::vec2(b.x, b.y), t);
    return Vector2(result.x, result.y);
}

Vector2 Min(const Vector2& a, const Vector2& b) {
    return Vector2(glm::min(a.x, b.x), glm::min(a.y, b.y));
}

Vector2 Max(const Vector2& a, const Vector2& b) {
    return Vector2(glm::max(a.x, b.x), glm::max(a.y, b.y));
}

Vector2 Clamp(const Vector2& v, const Vector2& min, const Vector2& max) {
    return Vector2(
        glm::clamp(v.x, min.x, max.x),
        glm::clamp(v.y, min.y, max.y)
    );
}

Vector2 Abs(const Vector2& v) {
    return Vector2(glm::abs(v.x), glm::abs(v.y));
}

} // namespace Zgine::Math
