#pragma once

#include <Zgine/Editor/MVVM/Observable.h>
#include <Zgine/Editor/MVVM/Command.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/World/Core/World.h>
#include <vector>
#include <unordered_set>
#include <memory>

namespace Zgine {

class EditorEventBus;
class EditorCommandHistory;
class SelectionContext;

/**
 * @brief ViewModel for World in MVVM pattern
 *
 * SceneViewModel sits between World (Model) and Editor UI (View).
 * It provides:
 * - Observable properties for data binding
 * - Commands for UI actions
 * - Presentation logic and validation
 * - Event handling and coordination
 */
class SceneViewModel {
public:
    SceneViewModel(
        World* World,
        EditorEventBus* eventBus,
        EditorCommandHistory* commandHistory,
        SelectionContext* selectionContext);

    ~SceneViewModel();

    // ========================================================================
    // Observable Properties (for View binding)
    // ========================================================================

    /**
     * @brief Currently selected entity
     */
    Observable<Entity> SelectedEntity;

    /**
     * @brief Total entity count in World
     */
    Observable<size_t> EntityCount;

    /**
     * @brief Whether World has unsaved changes
     */
    Observable<bool> IsDirty;

    /**
     * @brief World name/path
     */
    Observable<std::string> SceneName;

    // ========================================================================
    // Commands (for View actions)
    // ========================================================================

    /**
     * @brief Create new entity command
     */
    Command<std::string> CreateEntityCommand;

    /**
     * @brief Delete selected entity
     */
    SimpleCommand DeleteSelectedEntityCommand;

    /**
     * @brief Duplicate selected entity
     */
    SimpleCommand DuplicateSelectedEntityCommand;

    /**
     * @brief Save World
     */
    SimpleCommand SaveSceneCommand;

    /**
     * @brief Clear selection
     */
    SimpleCommand ClearSelectionCommand;

    // ========================================================================
    // Public Methods
    // ========================================================================

    /**
     * @brief Update ViewModel (call per frame)
     */
    void Update();

    /**
     * @brief Get all entities in World
     */
    std::vector<Entity> GetAllEntities() const;

    /**
     * @brief Get entities matching filter
     */
    template<typename... Components>
    std::vector<Entity> GetEntitiesWithComponents() const {
        std::vector<Entity> result;
        auto view = m_World->GetEntitiesWith<Components...>();
        for (auto entity : view) {
            result.emplace_back(entity, m_World);
        }
        return result;
    }

    /**
     * @brief Refresh all observable properties from model
     */
    void RefreshFromModel();

private:
    // Model references
    World* m_World;
    EditorEventBus* m_EventBus;
    EditorCommandHistory* m_CommandHistory;
    SelectionContext* m_SelectionContext;

    // Event subscriptions
    void SetupEventSubscriptions();
    void OnEntityCreated(Entity entity);
    void OnEntityDestroyed(Entity entity);
    void OnSelectionChanged();
    void OnSceneModified();

    // Command implementations
    void ExecuteCreateEntity(const std::string& name);
    void ExecuteDeleteSelectedEntity();
    void ExecuteDuplicateSelectedEntity();
    void ExecuteSaveScene();
    void ExecuteClearSelection();

    // State
    bool m_Initialized = false;
};

} // namespace Zgine
