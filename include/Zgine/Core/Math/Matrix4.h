#pragma once

#include <Zgine/Core/Math/Vector3.h>
#include <Zgine/Core/Math/Vector4.h>

namespace Zgine::Math {

/**
 * @brief Backend-agnostic 4x4 matrix
 */
struct Matrix4 {
    float m[16]; // Column-major order

    // Constructors
    Matrix4();
    explicit Matrix4(float diagonal);

    // Element access
    float& operator()(int row, int col);
    const float& operator()(int row, int col) const;

    // Operators
    Matrix4 operator*(const Matrix4& other) const;
    Vector4 operator*(const Vector4& v) const;
    Matrix4& operator*=(const Matrix4& other);

    // Static constructors
    static Matrix4 Identity();
    static Matrix4 Translation(const Vector3& translation);
    static Matrix4 Rotation(float angle, const Vector3& axis);
    static Matrix4 Scale(const Vector3& scale);
    static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
    static Matrix4 Perspective(float fovY, float aspect, float nearPlane, float farPlane);
    static Matrix4 Ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane);
};

// Matrix operations
Matrix4 Inverse(const Matrix4& m);
Matrix4 Transpose(const Matrix4& m);
Vector3 ExtractTranslation(const Matrix4& m);
Vector3 ExtractScale(const Matrix4& m);
const float* ValuePtr(const Matrix4& m);
float* ValuePtr(Matrix4& m);  // Non-const version for ImGuizmo

} // namespace Zgine::Math
