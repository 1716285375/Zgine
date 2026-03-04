#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
#endif

namespace Zgine::Math {

/**
 * @brief Backend-agnostic 3D vector with multiple accessor support
 */
struct Vector3 {
    union {
        struct { float x, y, z; };
        struct { float r, g, b; };
        float data[3];
    };

    // Constructors
    Vector3();
    explicit Vector3(float scalar);
    Vector3(float x, float y, float z);

    // Operators
    Vector3& operator=(const Vector3& other) = default;
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;
    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(float scalar);
    Vector3& operator/=(float scalar);
    Vector3 operator-() const;

    // Comparison operators (C++20)
    bool operator==(const Vector3& other) const;
    auto operator<=>(const Vector3& other) const;

    float& operator[](int index);
    const float& operator[](int index) const;

    // Static members
    static const Vector3 Zero;
    static const Vector3 One;
    static const Vector3 Up;
    static const Vector3 Down;
    static const Vector3 Left;
    static const Vector3 Right;
    static const Vector3 Forward;
    static const Vector3 Back;
};

// Vector operations
float Length(const Vector3& v);
float LengthSquared(const Vector3& v);
Vector3 Normalize(const Vector3& v);
float Dot(const Vector3& a, const Vector3& b);
Vector3 Cross(const Vector3& a, const Vector3& b);
Vector3 Lerp(const Vector3& a, const Vector3& b, float t);
Vector3 Min(const Vector3& a, const Vector3& b);
Vector3 Max(const Vector3& a, const Vector3& b);
Vector3 Clamp(const Vector3& v, const Vector3& min, const Vector3& max);

} // namespace Zgine::Math

#ifdef _MSC_VER
#pragma warning(pop)
#endif
