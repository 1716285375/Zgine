#pragma once

#include <Zgine/Scene/Core/Entity.h>

namespace Zgine {
namespace UI {
namespace Inspectors {

/**
 * @brief Physics component property inspectors
 */
class PhysicsInspector {
public:
    static void DrawRigidbody2DProperties(Entity entity);
    static void DrawBoxCollider2DProperties(Entity entity);
    static void DrawCircleCollider2DProperties(Entity entity);
};

} // namespace Inspectors
} // namespace UI
} // namespace Zgine
