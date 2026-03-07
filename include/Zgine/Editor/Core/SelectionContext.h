#pragma once

#include <vector>
#include <algorithm>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/Editor/Core/EditorTypes.h>

namespace Zgine {

// Forward declarations
class World;
class EditorEventBus;

/**
 * @brief Manages entity selection state in the editor
 *
 * SelectionContext tracks which entities are selected in the editor,
 * maintaining both a primary selection (for Inspector focus) and a
 * list of all selected entities (for batch operations).
 *
 * **Features**:
 * - Single and multi-selection support
 * - Selection modes (Replace, Add, Subtract, Toggle)
 * - Automatic event publishing (integrates with EditorEventBus)
 * - Selection validation against active World
 *
 * **Usage**:
 * @code
 *   SelectionContext selection(eventBus);
 *
 *   // Select single entity (replaces current selection)
 *   selection.Select(entity);
 *
 *   // Add to selection (multi-select)
 *   selection.Select(entity, SelectionMode::Add);
 *
 *   // Check if selected
 *   if (selection.IsSelected(entity)) {
 *       // ...
 *   }
 *
 *   // Get all selected entities
 *   for (Entity e : selection.GetSelection()) {
 *       // ...
 *   }
 * @endcode
 */
class SelectionContext {
public:
    /**
     * @brief Construct SelectionContext
     * @param eventBus Event bus for publishing selection events (optional)
     */
    explicit SelectionContext(EditorEventBus* eventBus = nullptr);
    ~SelectionContext() = default;

    //=========================================================================
    // Selection Operations
    //=========================================================================

    /**
     * @brief Select an entity
     * @param entity Entity to select
     * @param mode Selection mode (Replace, Add, Subtract, Toggle)
     */
    void Select(Entity entity, SelectionMode mode = SelectionMode::Replace);

    /**
     * @brief Select multiple entities
     * @param entities Entities to select
     * @param mode Selection mode
     */
    void Select(const std::vector<Entity>& entities, SelectionMode mode = SelectionMode::Replace);

    /**
     * @brief Clear all selection
     */
    void Clear();

    /**
     * @brief Set the primary selected entity
     *
     * The primary entity is shown in the Inspector.
     * If not already selected, it will be added to selection.
     *
     * @param entity Primary entity
     */
    void SetPrimary(Entity entity);

    /**
     * @brief Add entity to selection
     * @param entity Entity to add
     */
    void Add(Entity entity);

    /**
     * @brief Remove entity from selection
     * @param entity Entity to remove
     */
    void Remove(Entity entity);

    /**
     * @brief Toggle entity selection
     * @param entity Entity to toggle
     */
    void Toggle(Entity entity);

    //=========================================================================
    // Query Operations
    //=========================================================================

    /**
     * @brief Check if entity is selected
     * @param entity Entity to check
     * @return true if entity is selected
     */
    bool IsSelected(Entity entity) const;

    /**
     * @brief Get the primary selected entity
     * @return Primary entity (invalid if no selection)
     */
    Entity GetPrimary() const noexcept { return m_Primary; }

    /**
     * @brief Get all selected entities
     * @return Vector of selected entities
     */
    const std::vector<Entity>& GetSelection() const noexcept { return m_Selection; }

    /**
     * @brief Get number of selected entities
     * @return Selection count
     */
    size_t GetSelectionCount() const noexcept { return m_Selection.size(); }

    /**
     * @brief Check if there is any selection
     * @return true if at least one entity is selected
     */
    bool HasSelection() const noexcept { return !m_Selection.empty(); }

    /**
     * @brief Check if multiple entities are selected
     * @return true if more than one entity is selected
     */
    bool HasMultipleSelection() const noexcept { return m_Selection.size() > 1; }

    //=========================================================================
    // Validation & Cleanup
    //=========================================================================

    /**
     * @brief Validate selection against active World
     *
     * Removes any entities that no longer exist in the World.
     * Call this after loading a new World or when entities are deleted.
     *
     * @param World World to validate against
     */
    void Validate(World* World);

    //=========================================================================
    // Event Integration
    //=========================================================================

    /**
     * @brief Set event bus for automatic event publishing
     * @param eventBus Event bus to use
     */
    void SetEventBus(EditorEventBus* eventBus) { m_EventBus = eventBus; }

    /**
     * @brief Enable/disable automatic event publishing
     * @param enabled true to enable event publishing
     */
    void SetEventPublishingEnabled(bool enabled) { m_PublishEvents = enabled; }

private:
    void PublishSelectionChanged();
    void PublishDeselected();

    Entity m_Primary;                      ///< Primary selected entity (shown in Inspector)
    std::vector<Entity> m_Selection;       ///< All selected entities
    EditorEventBus* m_EventBus = nullptr;  ///< Event bus for publishing events
    bool m_PublishEvents = true;           ///< Whether to publish selection events
};

} // namespace Zgine
