#pragma once

#include <Zgine/Editor/Panels/EditorPanel.h>
#include <Zgine/Scene/Core/Entity.h>
#include <Zgine/Resources/Mesh/PrimitiveMesh.h>
#include <string>

namespace Zgine {

class Scene;

/**
 * @brief Hierarchy panel for scene entity tree view
 *
 * Displays all entities in the scene in a hierarchical tree structure.
 * Supports entity selection, creation, deletion, and drag-drop reparenting.
 *
 * **Features**:
 * - Hierarchical entity tree
 * - Entity search/filtering
 * - Context menu for entity operations
 * - Drag-and-drop reparenting
 * - Multi-selection support (future)
 *
 * **Keyboard Shortcuts**:
 * - Right-click: Open context menu
 * - Delete: Delete selected entity
 */
class HierarchyPanel : public EditorPanel {
public:
    HierarchyPanel(const std::string& name, EditorContext& context);
    ~HierarchyPanel() override = default;

    // EditorPanel lifecycle
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnGuiRender() override;

    /** Set the scene to display */
    void SetScene(Scene* scene) { m_Scene = scene; }

private:
    void DrawHierarchyNode(Scene* scene, Entity entity);
    bool PassHierarchyFilter(Scene* scene, Entity entity);
    void CreatePrimitive(Scene* scene, PrimitiveType type, Entity parent = {});
    void DeleteEntity(Scene* scene, Entity entity);

    Scene* m_Scene = nullptr;
    std::string m_SearchQuery;
};

} // namespace Zgine
