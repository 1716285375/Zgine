#pragma once

#include <Zgine/World/Core/Entity.h>

namespace Zgine {

class EditorCommandHistory;

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
     * @param commandHistory Optional command history for undoable edits
     */
    static void DrawTransformProperties(Entity entity, EditorCommandHistory* commandHistory = nullptr);

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
