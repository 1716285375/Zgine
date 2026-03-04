// GLM Backend Implementation for Vector4
#include <Zgine/Core/Math/Vector4.h>
#include <Zgine/Core/Math/Vector3.h>
#include <Zgine/Core/Math/Math.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace Zgine::Math {

// Static members
const Vector4 Vector4::Zero(0.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::One(1.0f, 1.0f, 1.0f, 1.0f);

// Constructors
Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
Vector4::Vector4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
Vector4::Vector4(const Vector3& vec3, float w) : x(vec3.x), y(vec3.y), z(vec3.z), w(w) {}
Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

// Operators
Vector4 Vector4::operator+(const Vector4& other) const {
    return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vector4 Vector4::operator-(const Vector4& other) const {
    return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vector4 Vector4::operator*(float scalar) const {
    return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vector4 Vector4::operator/(float scalar) const {
    return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
}

Vector4& Vector4::operator+=(const Vector4& other) {
    x += other.x; y += other.y; z += other.z; w += other.w;
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& other) {
    x -= other.x; y -= other.y; z -= other.z; w -= other.w;
    return *this;
}

Vector4& Vector4::operator*=(float scalar) {
    x *= scalar; y *= scalar; z *= scalar; w *= scalar;
    return *this;
}

Vector4& Vector4::operator/=(float scalar) {
    x /= scalar; y /= scalar; z /= scalar; w /= scalar;
    return *this;
}

Vector4 Vector4::operator-() const {
    return Vector4(-x, -y, -z, -w);
}

// Comparison operators
bool Vector4::operator==(const Vector4& other) const {
    return glm::abs(x - other.x) < EPSILON &&
           glm::abs(y - other.y) < EPSILON &&
           glm::abs(z - other.z) < EPSILON &&
           glm::abs(w - other.w) < EPSILON;
}

auto Vector4::operator<=>(const Vector4& other) const {
    // Lexicographic comparison: compare x, then y, then z, then w
    if (auto cmp = x <=> other.x; cmp != 0) return cmp;
    if (auto cmp = y <=> other.y; cmp != 0) return cmp;
    if (auto cmp = z <=> other.z; cmp != 0) return cmp;
    return w <=> other.w;
}

float& Vector4::operator[](int index) {
    return (&x)[index];
}

const float& Vector4::operator[](int index) const {
    return (&x)[index];
}

// Vector operations using GLM
float Length(const Vector4& v) {
    return glm::length(glm::vec4(v.x, v.y, v.z, v.w));
}

float LengthSquared(const Vector4& v) {
    return glm::length2(glm::vec4(v.x, v.y, v.z, v.w));
}

Vector4 Normalize(const Vector4& v) {
    glm::vec4 result = glm::normalize(glm::vec4(v.x, v.y, v.z, v.w));
    return Vector4(result.x, result.y, result.z, result.w);
}

float Dot(const Vector4& a, const Vector4& b) {
    return glm::dot(glm::vec4(a.x, a.y, a.z, a.w), glm::vec4(b.x, b.y, b.z, b.w));
}

Vector4 Lerp(const Vector4& a, const Vector4& b, float t) {
    glm::vec4 result = glm::mix(glm::vec4(a.x, a.y, a.z, a.w), glm::vec4(b.x, b.y, b.z, b.w), t);
    return Vector4(result.x, result.y, result.z, result.w);
}

Vector4 Min(const Vector4& a, const Vector4& b) {
    return Vector4(
        glm::min(a.x, b.x),
        glm::min(a.y, b.y),
        glm::min(a.z, b.z),
        glm::min(a.w, b.w)
    );
}

Vector4 Max(const Vector4& a, const Vector4& b) {
    return Vector4(
        glm::max(a.x, b.x),
        glm::max(a.y, b.y),
        glm::max(a.z, b.z),
        glm::max(a.w, b.w)
    );
}

Vector4 Clamp(const Vector4& v, const Vector4& min, const Vector4& max) {
    return Vector4(
        glm::clamp(v.x, min.x, max.x),
        glm::clamp(v.y, min.y, max.y),
        glm::clamp(v.z, min.z, max.z),
        glm::clamp(v.w, min.w, max.w)
    );
}

} // namespace Zgine::Math
