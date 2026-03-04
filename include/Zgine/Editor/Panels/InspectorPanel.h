#pragma once

#include <Zgine/Editor/Panels/EditorPanel.h>
#include <Zgine/Scene/Core/Entity.h>

namespace Zgine {

class Scene;

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

    /** Set the scene to inspect entities from */
    void SetScene(Scene* scene) { m_Scene = scene; }

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

    void DeleteEntity(Scene* scene, Entity entity);

    Scene* m_Scene = nullptr;
};

} // namespace Zgine
