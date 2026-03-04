// GLM Backend Implementation for Matrix4
#include <Zgine/Core/Math/Matrix4.h>
#include <Zgine/Core/Math/Vector3.h>
#include <Zgine/Core/Math/Vector4.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>

namespace Zgine::Math {

// Helper to convert our Matrix4 to glm::mat4
static glm::mat4 ToGLM(const Matrix4& m) {
    glm::mat4 result;
    std::memcpy(glm::value_ptr(result), m.m, 16 * sizeof(float));
    return result;
}

// Helper to convert glm::mat4 to our Matrix4
static Matrix4 FromGLM(const glm::mat4& m) {
    Matrix4 result;
    std::memcpy(result.m, glm::value_ptr(m), 16 * sizeof(float));
    return result;
}

// Constructors
Matrix4::Matrix4() {
    std::memset(m, 0, 16 * sizeof(float));
}

Matrix4::Matrix4(float diagonal) {
    std::memset(m, 0, 16 * sizeof(float));
    m[0] = diagonal;
    m[5] = diagonal;
    m[10] = diagonal;
    m[15] = diagonal;
}

// Element access (column-major: m[col * 4 + row])
float& Matrix4::operator()(int row, int col) {
    return m[col * 4 + row];
}

const float& Matrix4::operator()(int row, int col) const {
    return m[col * 4 + row];
}

// Operators
Matrix4 Matrix4::operator*(const Matrix4& other) const {
    return FromGLM(ToGLM(*this) * ToGLM(other));
}

Vector4 Matrix4::operator*(const Vector4& v) const {
    glm::vec4 result = ToGLM(*this) * glm::vec4(v.x, v.y, v.z, v.w);
    return Vector4(result.x, result.y, result.z, result.w);
}

Matrix4& Matrix4::operator*=(const Matrix4& other) {
    *this = *this * other;
    return *this;
}

// Static constructors
Matrix4 Matrix4::Identity() {
    return FromGLM(glm::mat4(1.0f));
}

Matrix4 Matrix4::Translation(const Vector3& translation) {
    return FromGLM(glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, translation.z)));
}

Matrix4 Matrix4::Rotation(float angle, const Vector3& axis) {
    return FromGLM(glm::rotate(glm::mat4(1.0f), angle, glm::vec3(axis.x, axis.y, axis.z)));
}

Matrix4 Matrix4::Scale(const Vector3& scale) {
    return FromGLM(glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z)));
}

Matrix4 Matrix4::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
    return FromGLM(glm::lookAt(
        glm::vec3(eye.x, eye.y, eye.z),
        glm::vec3(center.x, center.y, center.z),
        glm::vec3(up.x, up.y, up.z)
    ));
}

Matrix4 Matrix4::Perspective(float fovY, float aspect, float nearPlane, float farPlane) {
    return FromGLM(glm::perspective(fovY, aspect, nearPlane, farPlane));
}

Matrix4 Matrix4::Ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    return FromGLM(glm::ortho(left, right, bottom, top, nearPlane, farPlane));
}

// Matrix operations
Matrix4 Inverse(const Matrix4& m) {
    return FromGLM(glm::inverse(ToGLM(m)));
}

Matrix4 Transpose(const Matrix4& m) {
    return FromGLM(glm::transpose(ToGLM(m)));
}

Vector3 ExtractTranslation(const Matrix4& m) {
    return Vector3(m.m[12], m.m[13], m.m[14]);
}

Vector3 ExtractScale(const Matrix4& m) {
    glm::mat4 glmMat = ToGLM(m);
    return Vector3(
        glm::length(glm::vec3(glmMat[0])),
        glm::length(glm::vec3(glmMat[1])),
        glm::length(glm::vec3(glmMat[2]))
    );
}

const float* ValuePtr(const Matrix4& m) {
    return m.m;
}

float* ValuePtr(Matrix4& m) {
    return m.m;
}

} // namespace Zgine::Math
