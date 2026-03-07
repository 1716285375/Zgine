#pragma once

#include <Zgine/Editor/Panels/EditorPanel.h>
#include <Zgine/World/Core/Entity.h>
#include <Zgine/Resources/Mesh/PrimitiveMesh.h>
#include <string>

namespace Zgine {

class World;

/**
 * @brief Hierarchy panel for World entity tree view
 *
 * Displays all entities in the World in a hierarchical tree structure.
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

    /** Set the World to display */
    void SetScene(World* World) { m_World = World; }

private:
    void DrawHierarchyNode(World* World, Entity entity);
    bool PassHierarchyFilter(World* World, Entity entity);
    void CreatePrimitive(World* World, PrimitiveType type, Entity parent = {});
    void DeleteEntity(World* World, Entity entity);

    World* m_World = nullptr;
    std::string m_SearchQuery;
};

} // namespace Zgine
