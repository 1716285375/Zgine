// Example: How to use SceneViewModel in Editor Panels

#include <Zgine/Editor/ViewModels/SceneViewModel.h>

namespace Zgine {

class HierarchyPanel {
public:
    void Initialize(SceneViewModel* viewModel) {
        m_ViewModel = viewModel;

        // Subscribe to entity count changes
        m_ViewModel->EntityCount.Subscribe([this](size_t count) {
            // Update UI label
            m_EntityCountLabel = "Entities: " + std::to_string(count);
        });

        // Subscribe to selection changes
        m_ViewModel->SelectedEntity.Subscribe([this](Entity entity) {
            // Highlight selected entity in hierarchy
            UpdateSelectedEntityHighlight(entity);
        });

        // Subscribe to dirty flag
        m_ViewModel->IsDirty.Subscribe([this](bool dirty) {
            // Show unsaved indicator
            m_ShowUnsavedIndicator = dirty;
        });
    }

    void OnImGuiRender() {
        // Create Entity Button
        if (ImGui::Button("Create Entity")) {
            m_ViewModel->CreateEntityCommand.Execute("New Entity");
        }

        // Delete Entity Button (auto-disabled if nothing selected)
        ImGui::BeginDisabled(!m_ViewModel->DeleteSelectedEntityCommand.CanExecute());
        if (ImGui::Button("Delete Entity")) {
            m_ViewModel->DeleteSelectedEntityCommand.Execute();
        }
        ImGui::EndDisabled();

        // Save World Button
        if (ImGui::Button("Save") || ImGui::IsKeyPressed(ImGuiKey_S, ImGuiKeyModFlags_Ctrl)) {
            m_ViewModel->SaveSceneCommand.Execute();
        }

        // Display entity count (auto-updated via Observable)
        ImGui::Text("%s", m_EntityCountLabel.c_str());

        // Show dirty indicator
        if (m_ShowUnsavedIndicator) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "*");
        }
    }

private:
    SceneViewModel* m_ViewModel;
    std::string m_EntityCountLabel;
    bool m_ShowUnsavedIndicator = false;

    void UpdateSelectedEntityHighlight(Entity entity) {
        // Implementation...
    }
};

} // namespace Zgine

/*
 * Benefits of using SceneViewModel:
 *
 * 1. Reactive UI: Properties auto-update when model changes
 * 2. Clean Separation: Panel doesn't know about EditorCommandHistory
 * 3. Testable: Can mock ViewModel without UI
 * 4. Maintainable: Business logic in ViewModel, not scattered in UI
 * 5. Reusable: Same ViewModel for different Views
 */
