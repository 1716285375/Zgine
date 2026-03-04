// GLM Backend Implementation for Vector3
#include <Zgine/Core/Math/Vector3.h>
#include <Zgine/Core/Math/Math.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

namespace Zgine::Math {

// Static members
const Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::One(1.0f, 1.0f, 1.0f);
const Vector3 Vector3::Up(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::Down(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::Left(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::Right(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::Forward(0.0f, 0.0f, -1.0f);
const Vector3 Vector3::Back(0.0f, 0.0f, 1.0f);

// Constructors
Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
Vector3::Vector3(float scalar) : x(scalar), y(scalar), z(scalar) {}
Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

// Operators
Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar) const {
    return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3& Vector3::operator+=(const Vector3& other) {
    x += other.x; y += other.y; z += other.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& other) {
    x -= other.x; y -= other.y; z -= other.z;
    return *this;
}

Vector3& Vector3::operator*=(float scalar) {
    x *= scalar; y *= scalar; z *= scalar;
    return *this;
}

Vector3& Vector3::operator/=(float scalar) {
    x /= scalar; y /= scalar; z /= scalar;
    return *this;
}

Vector3 Vector3::operator-() const {
    return Vector3(-x, -y, -z);
}

// Comparison operators
bool Vector3::operator==(const Vector3& other) const {
    return glm::abs(x - other.x) < EPSILON &&
           glm::abs(y - other.y) < EPSILON &&
           glm::abs(z - other.z) < EPSILON;
}

auto Vector3::operator<=>(const Vector3& other) const {
    // Lexicographic comparison: compare x, then y, then z
    if (auto cmp = x <=> other.x; cmp != 0) return cmp;
    if (auto cmp = y <=> other.y; cmp != 0) return cmp;
    return z <=> other.z;
}

float& Vector3::operator[](int index) {
    return (&x)[index];
}

const float& Vector3::operator[](int index) const {
    return (&x)[index];
}

// Vector operations using GLM
float Length(const Vector3& v) {
    return glm::length(glm::vec3(v.x, v.y, v.z));
}

float LengthSquared(const Vector3& v) {
    return glm::length2(glm::vec3(v.x, v.y, v.z));
}

Vector3 Normalize(const Vector3& v) {
    glm::vec3 result = glm::normalize(glm::vec3(v.x, v.y, v.z));
    return Vector3(result.x, result.y, result.z);
}

float Distance(const Vector3& a, const Vector3& b) {
    return glm::distance(glm::vec3(a.x, a.y, a.z), glm::vec3(b.x, b.y, b.z));
}

float Dot(const Vector3& a, const Vector3& b) {
    return glm::dot(glm::vec3(a.x, a.y, a.z), glm::vec3(b.x, b.y, b.z));
}

Vector3 Cross(const Vector3& a, const Vector3& b) {
    glm::vec3 result = glm::cross(glm::vec3(a.x, a.y, a.z), glm::vec3(b.x, b.y, b.z));
    return Vector3(result.x, result.y, result.z);
}

Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
    glm::vec3 result = glm::mix(glm::vec3(a.x, a.y, a.z), glm::vec3(b.x, b.y, b.z), t);
    return Vector3(result.x, result.y, result.z);
}

Vector3 Reflect(const Vector3& direction, const Vector3& normal) {
    glm::vec3 result = glm::reflect(glm::vec3(direction.x, direction.y, direction.z),
                                     glm::vec3(normal.x, normal.y, normal.z));
    return Vector3(result.x, result.y, result.z);
}

Vector3 Min(const Vector3& a, const Vector3& b) {
    return Vector3(glm::min(a.x, b.x), glm::min(a.y, b.y), glm::min(a.z, b.z));
}

Vector3 Max(const Vector3& a, const Vector3& b) {
    return Vector3(glm::max(a.x, b.x), glm::max(a.y, b.y), glm::max(a.z, b.z));
}

Vector3 Clamp(const Vector3& v, const Vector3& min, const Vector3& max) {
    return Vector3(
        glm::clamp(v.x, min.x, max.x),
        glm::clamp(v.y, min.y, max.y),
        glm::clamp(v.z, min.z, max.z)
    );
}

Vector3 Abs(const Vector3& v) {
    return Vector3(glm::abs(v.x), glm::abs(v.y), glm::abs(v.z));
}

} // namespace Zgine::Math
