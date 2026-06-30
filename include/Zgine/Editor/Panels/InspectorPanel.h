#pragma once

#include <Zgine/Editor/Panels/EditorPanel.h>
#include <Zgine/World/Core/Entity.h>

namespace Zgine {

class World;
struct AssetSelection;

/**
 * @brief Inspector panel for editing entity components
 *
 * Displays and allows editing of all components attached to
 * the currently selected entity.
 *
 * **Features**:
 * - Real-time component property editing
 * - Add/remove components
 * - Component-specific UI widgets
 * - Undo/redo support for property changes
 * - Delete entity button
 *
 * **Supported Components**:
 * - Tag, Transform, Color
 * - RigidBody, BoxCollider
 * - AudioSource, AudioListener
 * - Script, PBRMaterial
 * - DirectionalLight, PointLight, SpotLight
 */
class InspectorPanel : public EditorPanel {
public:
    InspectorPanel(const std::string& name, EditorContext& context);
    ~InspectorPanel() override = default;

    // EditorPanel lifecycle
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnGuiRender() override;

    /** Set the World to inspect entities from */
    void SetScene(World* World) { m_World = World; }

private:
    /**
     * @brief Draw a component inspector with optional remove menu
     * @tparam Component The component type to draw
     * @param entity Entity containing the component
     * @param drawFunc Function to draw the component properties
     */
    template<typename Component>
    void DrawComponentInspector(Entity entity, void (*drawFunc)(Entity));

    /**
     * @brief Add menu item to add a component
     * @tparam Component The component type to add
     * @param entity Entity to add component to
     * @param label Menu item label
     */
    template<typename Component>
    void AddComponentMenuItem(Entity entity, const char* label);

    void DeleteEntity(World* World, Entity entity);
    void DrawAssetInspector(const AssetSelection& selection);

    World* m_World = nullptr;
};

} // namespace Zgine
