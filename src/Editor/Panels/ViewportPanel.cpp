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
#include <Zgine/Core/Math/MathTypes.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include <entt/entt.hpp>

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
	}

	void ViewportPanel::OnDetach() {
	}

	void ViewportPanel::OnUpdate(float deltaTime) {
		ZGINE_UNUSED(deltaTime);
	}

	// Ray-AABB intersection test (slab method)
	static bool RayIntersectsAABB(const Math::Vector3& rayOrigin, const Math::Vector3& rayDir,
	                               const Math::Vector3& aabbMin, const Math::Vector3& aabbMax,
	                               float& tOut) {
		float tmin = -1e30f;
		float tmax = 1e30f;

		for (int i = 0; i < 3; ++i) {
			float dir = rayDir[i];
			float orig = rayOrigin[i];
			float bmin = aabbMin[i];
			float bmax = aabbMax[i];

			if (std::abs(dir) < 1e-8f) {
				if (orig < bmin || orig > bmax) return false;
			} else {
				float invD = 1.0f / dir;
				float t1 = (bmin - orig) * invD;
				float t2 = (bmax - orig) * invD;
				if (t1 > t2) std::swap(t1, t2);
				tmin = std::max(tmin, t1);
				tmax = std::min(tmax, t2);
				if (tmin > tmax) return false;
			}
		}

		tOut = tmin > 0.0f ? tmin : tmax;
		return tOut > 0.0f;
	}

	void ViewportPanel::HandleMousePicking() {
		if (!m_World) return;

		auto& viewportCtx = GetContext().GetViewportContext();
		if (!viewportCtx.IsViewProjectionValid()) return;

		// Don't pick while gizmo is being used or hovered
		if (ImGuizmo::IsOver() || ImGuizmo::IsUsing()) return;

		// Only pick on left click in viewport
		if (!ImGui::IsMouseClicked(ImGuiMouseButton_Left)) return;
		if (!ImGui::IsWindowHovered()) return;

		// Get mouse position relative to viewport
		ImVec2 mousePos = ImGui::GetMousePos();
		Math::Vector2 boundsMin = viewportCtx.GetViewportBoundsMin();
		Math::Vector2 boundsMax = viewportCtx.GetViewportBoundsMax();

		float viewportW = boundsMax.x - boundsMin.x;
		float viewportH = boundsMax.y - boundsMin.y;
		if (viewportW < 1.0f || viewportH < 1.0f) return;

		// Normalized device coordinates [-1, 1]
		float ndcX = ((mousePos.x - boundsMin.x) / viewportW) * 2.0f - 1.0f;
		float ndcY = -(((mousePos.y - boundsMin.y) / viewportH) * 2.0f - 1.0f); // flip Y

		// Unproject to world space ray
		Math::Matrix4 invVP = Math::Inverse(viewportCtx.GetProjection() * viewportCtx.GetView());

		Math::Vector4 nearPoint = invVP * Math::Vector4(ndcX, ndcY, -1.0f, 1.0f);
		Math::Vector4 farPoint  = invVP * Math::Vector4(ndcX, ndcY,  1.0f, 1.0f);

		if (std::abs(nearPoint.w) < 1e-8f || std::abs(farPoint.w) < 1e-8f) return;

		Math::Vector3 rayOrigin(nearPoint.x / nearPoint.w, nearPoint.y / nearPoint.w, nearPoint.z / nearPoint.w);
		Math::Vector3 rayEnd(farPoint.x / farPoint.w, farPoint.y / farPoint.w, farPoint.z / farPoint.w);
		Math::Vector3 rayDir = Math::Normalize(rayEnd - rayOrigin);

		// Test all entities with TransformComponent
		Entity closestEntity;
		float closestT = 1e30f;

		auto& registry = m_World->GetRegistry();
		auto view = registry.view<TransformComponent>();
		for (auto enttId : view) {
			auto& tc = view.get<TransformComponent>(enttId);

			// Build AABB from transform (base unit cube [-0.5, 0.5] scaled and translated)
			Math::Vector3 halfExtents = tc.Scale * 0.5f;
			Math::Vector3 aabbMin = tc.Translation - halfExtents;
			Math::Vector3 aabbMax = tc.Translation + halfExtents;

			// Ensure minimum pickable size
			for (int i = 0; i < 3; ++i) {
				if (aabbMax[i] - aabbMin[i] < 0.2f) {
					aabbMin[i] = tc.Translation[i] - 0.1f;
					aabbMax[i] = tc.Translation[i] + 0.1f;
				}
			}

			float t = 0.0f;
			if (RayIntersectsAABB(rayOrigin, rayDir, aabbMin, aabbMax, t)) {
				if (t < closestT) {
					closestT = t;
					closestEntity = Entity(enttId, m_World);
				}
			}
		}

		// Update selection
		auto& selection = GetContext().GetSelectionContext();
		if (closestEntity) {
			selection.Select(closestEntity);
		} else {
			selection.Clear();
		}
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

		// Allow ImGuizmo to receive mouse events on top of the Image widget
		ImGui::SetItemAllowOverlap();

		// Handle asset drag-drop
		if (m_World && ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ZGINE_ASSET_PATH")) {
				const char* path = static_cast<const char*>(payload->Data);
				if (path) {
					AssetDroppedEvent event(m_World, std::filesystem::path(path));
					GetContext().GetEventBus().PublishImmediate(event);
				}
			}
			ImGui::EndDragDropTarget();
		}

		// Gizmo rendering
		Entity selected = GetContext().GetSelectionContext().GetPrimary();
		if (viewportCtx.IsViewProjectionValid() && selected && selected.HasComponent<TransformComponent>()) {
			// Sync gizmo state from ViewportContext
			int gizmoOp = viewportCtx.GetGizmoOperation();
			if (gizmoOp == static_cast<int>(ImGuizmo::ROTATE))
				m_GizmoController->SetOperation(GizmoOperation::Rotate);
			else if (gizmoOp == static_cast<int>(ImGuizmo::SCALE))
				m_GizmoController->SetOperation(GizmoOperation::Scale);
			else
				m_GizmoController->SetOperation(GizmoOperation::Translate);

			int gizmoMode = viewportCtx.GetGizmoMode();
			m_GizmoController->SetSpace(gizmoMode == static_cast<int>(ImGuizmo::WORLD) ? GizmoSpace::World : GizmoSpace::Local);

			// Render gizmo
			m_GizmoController->Render(selected,
				viewportCtx.GetView(),
				viewportCtx.GetProjection(),
				viewportCtx.GetViewportBoundsMin(),
				viewportCtx.GetViewportBoundsMax());
		}

		// Mouse picking (after gizmo so ImGuizmo::IsOver/IsUsing are up-to-date)
		HandleMousePicking();

		EndPanel();
		ImGui::PopStyleVar();
	}

} // namespace Zgine
