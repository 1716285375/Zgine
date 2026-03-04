#pragma once

/**
 * @brief Main math header - includes all math utilities
 *
 * This is a convenience header that includes all math types and utilities.
 * Use this for full math functionality.
 */

#include <Zgine/Core/Math/Math.h>
#include <Zgine/Core/Math/Vector2.h>
#include <Zgine/Core/Math/Vector3.h>
#include <Zgine/Core/Math/Vector4.h>
#include <Zgine/Core/Math/Matrix3.h>
#include <Zgine/Core/Math/Matrix4.h>

namespace Zgine::Math {

// Convenience aliases
using Vec2 = Vector2;
using Vec3 = Vector3;
using Vec4 = Vector4;
using Mat3 = Matrix3;
using Mat4 = Matrix4;

} // namespace Zgine::Math
