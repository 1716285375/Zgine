#pragma once

#include <Zgine/Scene/Core/Entity.h>

namespace Zgine {
namespace UI {
namespace Inspectors {

/**
 * @brief Core component property inspectors
 *
 * Provides property editors for core engine components like
 * Transform, Tag, and Relationship.
 */
class CoreInspector {
public:
    /**
     * @brief Draw Transform component properties
     * @param entity Entity with TransformComponent
     */
    static void DrawTransformProperties(Entity entity);

    /**
     * @brief Draw Tag component properties
     * @param entity Entity with TagComponent
     */
    static void DrawTagProperties(Entity entity);

    /**
     * @brief Draw Relationship component properties
     * @param entity Entity with RelationshipComponent
     */
    static void DrawRelationshipProperties(Entity entity);
};

} // namespace Inspectors
} // namespace UI
} // namespace Zgine
