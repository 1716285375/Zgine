#pragma once

#include <Zgine/Scene/Core/Entity.h>
#include <span>

namespace Zgine {

// Forward declarations
class SelectionContext;

} // namespace Zgine

namespace Zgine::UI::Utils {

/**
 * @brief Handle entity click with modifier keys
 *
 * Automatically handles Ctrl/Shift for additive selection and toggles.
 *
 * @param ctx Selection context
 * @param entity Entity that was clicked
 * @param forceReplace If true, ignores modifiers and replaces selection
 */
void HandleEntityClick(SelectionContext& ctx, Entity entity, bool forceReplace = false);

/**
 * @brief Select multiple entities with selection mode
 *
 * @param ctx Selection context
 * @param entities Entities to select
 * @param additive If true, adds to selection; if false, replaces
 */
void SelectMultiple(SelectionContext& ctx, const std::vector<Entity>& entities, bool additive = false);

/**
 * @brief Toggle entity selection
 *
 * If selected, deselect; if not selected, select.
 *
 * @param ctx Selection context
 * @param entity Entity to toggle
 */
void ToggleEntity(SelectionContext& ctx, Entity entity);

/**
 * @brief Check if any modifier keys are pressed
 *
 * @return true if Ctrl or Shift is pressed
 */
bool IsAnyModifierPressed();

/**
 * @brief Check if additive selection is active (Ctrl or Shift)
 *
 * @return true if Ctrl or Shift is pressed
 */
inline bool IsAdditiveSelection() { return IsAnyModifierPressed(); }

} // namespace Zgine::UI::Utils
