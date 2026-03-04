#include <Zgine/Editor/Panels/StatusPanel.h>
#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Scene/Core/Scene.h>
#include <Zgine/Scene/Components/Components.h>
#include <imgui.h>

namespace Zgine
{

    StatusPanel::StatusPanel(const std::string &name, EditorContext& context)
        : EditorPanel(name, context), m_SelectionContext(context.GetSelectionContext()) {}

    void StatusPanel::OnAttach() {
        // Initialize status tracking if needed
    }

    void StatusPanel::OnDetach() {
        // Cleanup status tracking if needed
    }

    void StatusPanel::OnUpdate(float deltaTime) {
        ZGINE_UNUSED(deltaTime);
        // Status panel doesn't need per-frame updates
    }

    void StatusPanel::OnGuiRender()
    {
        BeginPanel();

        Entity selected = m_SelectionContext.GetPrimary();
        if (selected && selected.HasComponent<TagComponent>())
        {
            ImGui::Text("Selected: %s", selected.GetComponent<TagComponent>().Tag.c_str());
        }
        else
        {
            ImGui::Text("Selected: None");
        }

        if (m_Scene)
        {
            auto &registry = m_Scene->GetRegistry();
            auto &entities = registry.storage<entt::entity>();
            ImGui::Text("Entities: %zu", entities.size());
        }

        EndPanel();
    }

}
