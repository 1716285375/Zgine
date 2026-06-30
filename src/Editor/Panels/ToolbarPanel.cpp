#include <Zgine/Editor/Panels/ToolbarPanel.h>
#include <Zgine/Editor/Core/EditorContext.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

namespace Zgine {

namespace {

constexpr float kToolbarPaddingScale = 0.7f;

float GetToolbarPaddingY() {
    return ImGui::GetStyle().WindowPadding.y * kToolbarPaddingScale;
}

} // anonymous namespace

ToolbarPanel::ToolbarPanel(EditorContext& context)
    : EditorPanel("Toolbar", context, true)
{
    SetClosable(false);  // Toolbar should always be visible
    SetFloatable(false); // Toolbar should not float
}

void ToolbarPanel::OnAttach() {
    // Setup toolbar if needed
}

void ToolbarPanel::OnDetach() {
    // Cleanup toolbar if needed
}

void ToolbarPanel::OnUpdate(float deltaTime) {
    ZGINE_UNUSED(deltaTime);
    // Toolbar doesn't need per-frame updates
}

void ToolbarPanel::SetPlayModeCallback(const std::function<void(EditorMode)>& callback) {
    m_OnPlayMode = callback;
}

float ToolbarPanel::GetHeight() const {
    const float paddingY = GetToolbarPaddingY();
    return ImGui::GetFrameHeight() + paddingY * 2.0f;
}

void ToolbarPanel::OnGuiRender() {
    if (!m_ModeRef) return;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    if (!viewport) {
        return;
    }

    const float paddingY = GetToolbarPaddingY();
    const float toolbarHeight = ImGui::GetFrameHeight() + paddingY * 2.0f;
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + ImGui::GetFrameHeight()));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbarHeight));
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;

    const ImVec2 toolbarPadding(ImGui::GetStyle().WindowPadding.x, paddingY);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, toolbarPadding);
    ImGui::Begin("##Toolbar", nullptr, flags);

    auto setMode = [&](EditorMode nextMode) {
        if (*m_ModeRef == nextMode) {
            return;
        }
        if (m_OnPlayMode) {
            m_OnPlayMode(nextMode);
        } else {
            *m_ModeRef = nextMode;
        }
    };

    // Play/Pause/Stop controls
    if (*m_ModeRef == EditorMode::Edit) {
        if (ImGui::Button("Play")) {
            setMode(EditorMode::Play);
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(true);
        ImGui::Button("Pause");
        ImGui::SameLine();
        ImGui::Button("Stop");
        ImGui::EndDisabled();
    } else if (*m_ModeRef == EditorMode::Play) {
        if (ImGui::Button("Stop")) {
            setMode(EditorMode::Edit);
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
            setMode(EditorMode::Pause);
        }
    } else { // Paused
        if (ImGui::Button("Stop")) {
            setMode(EditorMode::Edit);
        }
        ImGui::SameLine();
        if (ImGui::Button("Resume")) {
            setMode(EditorMode::Play);
        }
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    // Gizmo operation
    if (m_GizmoOpRef) {
        ImGui::TextUnformatted("Gizmo");
        ImGui::SameLine();
        ImGui::RadioButton("Move", m_GizmoOpRef, ImGuizmo::TRANSLATE);
        ImGui::SameLine();
        ImGui::RadioButton("Rotate", m_GizmoOpRef, ImGuizmo::ROTATE);
        ImGui::SameLine();
        ImGui::RadioButton("Scale", m_GizmoOpRef, ImGuizmo::SCALE);

        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
    }

    // Gizmo mode
    if (m_GizmoModeRef) {
        ImGui::TextUnformatted("Space");
        ImGui::SameLine();
        ImGui::RadioButton("Local", m_GizmoModeRef, ImGuizmo::LOCAL);
        ImGui::SameLine();
        ImGui::RadioButton("World", m_GizmoModeRef, ImGuizmo::WORLD);

        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
    }

    // Grid and Axes toggles
    if (m_ShowGridRef) {
        ImGui::Checkbox("Grid", m_ShowGridRef);
        ImGui::SameLine();
    }
    if (m_ShowAxesRef) {
        ImGui::Checkbox("Axes", m_ShowAxesRef);
        ImGui::SameLine();
    }

    if (m_ShowGridRef || m_ShowAxesRef) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
    }

    // Current mode display
    const char* modeLabel = (*m_ModeRef == EditorMode::Edit) ? "Edit" :
                           (*m_ModeRef == EditorMode::Play) ? "Play" : "Pause";
    ImGui::Text("Mode: %s", modeLabel);

    ImGui::End();
    ImGui::PopStyleVar();
}

} // namespace Zgine
