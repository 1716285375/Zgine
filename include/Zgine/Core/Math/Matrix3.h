#pragma once

#include <Zgine/Core/Math/Vector3.h>

namespace Zgine::Math {

struct Matrix4; // Forward declaration

/**
 * @brief Backend-agnostic 3x3 matrix
 */
struct Matrix3 {
    float m[9]; // Column-major order

    // Constructors
    Matrix3();
    explicit Matrix3(float diagonal);

    // Element access
    float& operator()(int row, int col);
    const float& operator()(int row, int col) const;

    // Operators
    Matrix3 operator*(const Matrix3& other) const;
    Vector3 operator*(const Vector3& v) const;
    Matrix3& operator*=(const Matrix3& other);

    // Static constructors
    static Matrix3 Identity();
};

// Matrix operations
Matrix3 Inverse(const Matrix3& m);
Matrix3 Transpose(const Matrix3& m);
const float* ValuePtr(const Matrix3& m);

// Conversion from Matrix4 (defined after Matrix4 is complete)
Matrix3 ToMatrix3(const Matrix4& m);

} // namespace Zgine::Math
