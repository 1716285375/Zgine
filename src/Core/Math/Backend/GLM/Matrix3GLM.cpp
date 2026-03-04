// GLM Backend Implementation for Matrix3
#include <Zgine/Core/Math/Matrix3.h>
#include <Zgine/Core/Math/Matrix4.h>
#include <Zgine/Core/Math/Vector3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>

namespace Zgine::Math {

// Helper to convert our Matrix3 to glm::mat3
static glm::mat3 ToGLM(const Matrix3& m) {
    glm::mat3 result;
    std::memcpy(glm::value_ptr(result), m.m, 9 * sizeof(float));
    return result;
}

// Helper to convert glm::mat3 to our Matrix3
static Matrix3 FromGLM(const glm::mat3& m) {
    Matrix3 result;
    std::memcpy(result.m, glm::value_ptr(m), 9 * sizeof(float));
    return result;
}

// Constructors
Matrix3::Matrix3() {
    std::memset(m, 0, 9 * sizeof(float));
}

Matrix3::Matrix3(float diagonal) {
    std::memset(m, 0, 9 * sizeof(float));
    m[0] = diagonal;
    m[4] = diagonal;
    m[8] = diagonal;
}

// Element access (column-major: m[col * 3 + row])
float& Matrix3::operator()(int row, int col) {
    return m[col * 3 + row];
}

const float& Matrix3::operator()(int row, int col) const {
    return m[col * 3 + row];
}

// Operators
Matrix3 Matrix3::operator*(const Matrix3& other) const {
    return FromGLM(ToGLM(*this) * ToGLM(other));
}

Vector3 Matrix3::operator*(const Vector3& v) const {
    glm::vec3 result = ToGLM(*this) * glm::vec3(v.x, v.y, v.z);
    return Vector3(result.x, result.y, result.z);
}

Matrix3& Matrix3::operator*=(const Matrix3& other) {
    *this = *this * other;
    return *this;
}

// Static constructors
Matrix3 Matrix3::Identity() {
    return FromGLM(glm::mat3(1.0f));
}

// Matrix operations
Matrix3 Inverse(const Matrix3& m) {
    return FromGLM(glm::inverse(ToGLM(m)));
}

Matrix3 Transpose(const Matrix3& m) {
    return FromGLM(glm::transpose(ToGLM(m)));
}

const float* ValuePtr(const Matrix3& m) {
    return m.m;
}

Matrix3 ToMatrix3(const Matrix4& m4) {
    Matrix3 result;
    // Extract 3x3 from top-left of 4x4 matrix
    result.m[0] = m4.m[0]; result.m[1] = m4.m[1]; result.m[2] = m4.m[2];
    result.m[3] = m4.m[4]; result.m[4] = m4.m[5]; result.m[5] = m4.m[6];
    result.m[6] = m4.m[8]; result.m[7] = m4.m[9]; result.m[8] = m4.m[10];
    return result;
}

} // namespace Zgine::Math
