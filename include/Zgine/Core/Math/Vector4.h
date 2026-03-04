#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
#endif

namespace Zgine::Math {

struct Vector3;

/**
 * @brief Backend-agnostic 4D vector with multiple accessor support
 */
struct Vector4 {
    union {
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
        float data[4];
    };

    // Constructors
    Vector4();
    explicit Vector4(float scalar);
    Vector4(float x, float y, float z, float w);
    Vector4(const Vector3& vec3, float w);

    // Operators
    Vector4 operator+(const Vector4& other) const;
    Vector4 operator-(const Vector4& other) const;
    Vector4 operator*(float scalar) const;
    Vector4 operator/(float scalar) const;
    Vector4& operator+=(const Vector4& other);
    Vector4& operator-=(const Vector4& other);
    Vector4& operator*=(float scalar);
    Vector4& operator/=(float scalar);
    Vector4 operator-() const;

    // Comparison operators (C++20)
    bool operator==(const Vector4& other) const;
    auto operator<=>(const Vector4& other) const;

    float& operator[](int index);
    const float& operator[](int index) const;

    // Static members
    static const Vector4 Zero;
    static const Vector4 One;
};

// Vector operations
float Length(const Vector4& v);
float LengthSquared(const Vector4& v);
Vector4 Normalize(const Vector4& v);
float Dot(const Vector4& a, const Vector4& b);
Vector4 Lerp(const Vector4& a, const Vector4& b, float t);
Vector4 Min(const Vector4& a, const Vector4& b);
Vector4 Max(const Vector4& a, const Vector4& b);
Vector4 Clamp(const Vector4& v, const Vector4& min, const Vector4& max);

} // namespace Zgine::Math

#ifdef _MSC_VER
#pragma warning(pop)
#endif
