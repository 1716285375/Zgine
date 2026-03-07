#include <Zgine/Editor/Panels/ViewportPanel.h>
#include <Zgine/Editor/Core/EditorContext.h>
#include <Zgine/Editor/Core/EditorEventBus.h>
#include <Zgine/Editor/Core/SelectionContext.h>
#include <Zgine/Editor/Events/EntityEvents.h>
#include <Zgine/Editor/Events/SceneEvents.h>
#include <Zgine/Editor/Events/AssetEvents.h>
#include <Zgine/World/Core/World.h>
#include <Zgine/World/Components/Components.h>
#include <Zgine/Editor/Gizmo/GizmoController.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace Zgine {

	ViewportPanel::ViewportPanel(const std::string& name, EditorContext& context)
		: EditorPanel(name, context)
		, m_World(nullptr)
		, m_DockId(0)
		, m_GizmoController(std::make_unique<GizmoController>(context))
	{
	}

	ViewportPanel::~ViewportPanel() = default;

	void ViewportPanel::OnAttach() {
		// Gizmo controller is already initialized with EditorContext
		// No need for manual callback setup - it uses EventBus automatically
	}

	void ViewportPanel::OnDetach() {
		// Cleanup gizmo callbacks if needed
	}

	void ViewportPanel::OnUpdate(float deltaTime) {
		ZGINE_UNUSED(deltaTime);
		// Viewport doesn't need per-frame updates
	}

	void ViewportPanel::OnGuiRender() {
		auto& viewportCtx = GetContext().GetViewportContext();

		if (m_DockId != 0) {
			ImGui::SetNextWindowDockID(m_DockId, ImGuiCond_FirstUseEver);
		}

		ImGuiWindowFlags sceneFlags = ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		BeginPanel(sceneFlags);

		// Update viewport state in context
		viewportCtx.SetFocused(ImGui::IsWindowFocused());
		viewportCtx.SetHovered(ImGui::IsWindowHovered());

		// Calculate viewport bounds
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
		ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
		Math::Vector2 boundsMin = { windowPos.x + contentMin.x, windowPos.y + contentMin.y };
		Math::Vector2 boundsMax = { windowPos.x + contentMax.x, windowPos.y + contentMax.y };
		viewportCtx.SetViewportBounds(boundsMin, boundsMax);

		// Update viewport size
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (viewportPanelSize.x < 1.0f || viewportPanelSize.y < 1.0f) {
			viewportPanelSize = ImVec2(1.0f, 1.0f);
		}
		viewportCtx.SetViewportSize({ viewportPanelSize.x, viewportPanelSize.y });

		// Render World texture
		unsigned int textureId = viewportCtx.GetSceneTexture();
		if (textureId != 0) {
			ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(textureId)),
				viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));
		}
		else {
			ImGui::Text("World viewport is not ready.");
		}

		// Handle asset drag-drop
		if (m_World && ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ZGINE_ASSET_PATH")) {
				const char* path = static_cast<const char*>(payload->Data);
				if (path) {
					// Publish event instead of callback
					AssetDroppedEvent event(m_World, std::filesystem::path(path));
					GetContext().GetEventBus().PublishImmediate(event);
				}
			}
			ImGui::EndDragDropTarget();
		}

		// Gizmo rendering
		Entity selected = GetContext().GetSelectionContext().GetPrimary();
		if (viewportCtx.IsViewProjectionValid() && selected && selected.HasComponent<TransformComponent>()) {
			// Sync gizmo state from context
			// Note: GizmoOperation and GizmoMode would need to be added to ViewportContext
			// For now, using default values
			m_GizmoController->SetOperation(GizmoOperation::Translate);
			m_GizmoController->SetSpace(GizmoSpace::Local);

			// Render gizmo (ViewportContext returns Math types directly)
			m_GizmoController->Render(selected,
				viewportCtx.GetView(),
			viewportCtx.GetProjection(),
			viewportCtx.GetViewportBoundsMin(),
			viewportCtx.GetViewportBoundsMax());
		}

		EndPanel();
		ImGui::PopStyleVar();
	}

} // namespace Zgine
