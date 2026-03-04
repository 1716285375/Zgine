#pragma once

#include <Zgine/Scene/Core/Entity.h>
#include <functional>
#include <string>

namespace Zgine {

// Forward declarations
class Scene;
class SelectionContext;

} // namespace Zgine

namespace Zgine::UI::Widgets {

/**
 * @brief Reusable entity hierarchy tree widget
 *
 * Renders a hierarchical tree of entities with search/filter support,
 * drag & drop, multi-selection, and context menus via callbacks.
 */
class EntityTree {
public:
    EntityTree() = default;
    ~EntityTree() = default;

    // Configuration
    void SetScene(Scene* scene) { m_Scene = scene; }
    void SetSelectionContext(SelectionContext* ctx) { m_SelectionContext = ctx; }
    void SetFilter(const std::string& filter) { m_Filter = filter; }
    void SetDragDropEnabled(bool enabled) { m_DragDropEnabled = enabled; }

    // Callbacks
    std::function<void(Entity)> OnEntityClicked;
    std::function<void(Entity)> OnEntityDoubleClicked;
    std::function<bool(Entity)> OnEntityContextMenu;
    std::function<void(Entity dragged, Entity target)> OnEntityDragDrop;
    std::function<std::string(Entity)> CustomLabelRenderer;

    // Rendering
    void Render();
    void RenderNode(Entity entity);

private:
    bool PassFilter(Entity entity) const;
    void DrawEntityNode(Entity entity);
    std::string GetEntityLabel(Entity entity) const;

    Scene* m_Scene = nullptr;
    SelectionContext* m_SelectionContext = nullptr;
    std::string m_Filter;
    bool m_DragDropEnabled = true;
};

} // namespace Zgine::UI::Widgets
