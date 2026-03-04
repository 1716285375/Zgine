#pragma once

namespace Zgine::Math {

/**
 * @brief Backend-agnostic 2D vector
 */
struct Vector2 {
    float x, y;

    // Constructors
    Vector2();
    Vector2(float scalar);
    Vector2(float x, float y);

    // Operators
    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;
    Vector2& operator+=(const Vector2& other);
    Vector2& operator-=(const Vector2& other);
    Vector2& operator*=(float scalar);
    Vector2& operator/=(float scalar);
    Vector2 operator-() const;

    // Comparison operators (C++20)
    bool operator==(const Vector2& other) const;
    auto operator<=>(const Vector2& other) const;

    float& operator[](int index);
    const float& operator[](int index) const;

    // Static members
    static const Vector2 Zero;
    static const Vector2 One;
    static const Vector2 UnitX;
    static const Vector2 UnitY;
};

// Vector operations
float Length(const Vector2& v);
float LengthSquared(const Vector2& v);
Vector2 Normalize(const Vector2& v);
float Distance(const Vector2& a, const Vector2& b);
float Dot(const Vector2& a, const Vector2& b);
Vector2 Lerp(const Vector2& a, const Vector2& b, float t);
Vector2 Min(const Vector2& a, const Vector2& b);
Vector2 Max(const Vector2& a, const Vector2& b);
Vector2 Clamp(const Vector2& v, const Vector2& min, const Vector2& max);
Vector2 Abs(const Vector2& v);

} // namespace Zgine::Math
