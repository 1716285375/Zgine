#include <imgui_internal.h>

#include <Zgine/Editor/Panels/EditorPanel.h>
#include <Zgine/Editor/Core/EditorContext.h>

namespace Zgine {

	EditorPanel::EditorPanel(const std::string& name, EditorContext& context, bool openByDefault)
	: m_Name(name)
	, m_Icon()
	, m_IsOpen(openByDefault)
	, m_IsFocused(false)
	, m_IsHovered(false)
	, m_IsDocked(false)
	, m_WindowID(0)
	, m_IsClosable(true)
	, m_CanFloat(true)
	, m_MinWidth(0.0f)
	, m_MinHeight(0.0f)
	, m_IsDirty(false)
	, m_Context(context)
	{}

	EditorPanel::~EditorPanel() = default;

	void EditorPanel::OnUpdate(float deltaTime) {
		ZGINE_UNUSED(deltaTime);
	}

	bool EditorPanel::IsOpen() const {
		return m_IsOpen;
	}

	const std::string& EditorPanel::GetName() const {
		return m_Name;
	}

	void EditorPanel::SetOpen(bool open) {
		if (!open && !m_IsClosable) {
			return;
		}
		m_IsOpen = open;
	}

	bool EditorPanel::IsDocked() const {
		return m_IsDocked;
	}

	void EditorPanel::SetClosable(bool closable) {
		m_IsClosable = closable;
	}

	bool EditorPanel::IsClosable() const {
		return m_IsClosable;
	}

	void EditorPanel::SetFloatable(bool floatable) {
		m_CanFloat = floatable;
	}

	bool EditorPanel::IsFloatable() const {
		return m_CanFloat;
	}

	ImGuiID EditorPanel::GetWindowID() const {
		return m_WindowID;
	}

	void EditorPanel::BeginPanel(ImGuiWindowFlags flags) {
		ImGuiWindowFlags windowFlags = flags | ImGuiWindowFlags_NoCollapse;

		// Configure window class for docking behavior
		ImGuiWindowClass windowClass = {};
		windowClass.DockingAlwaysTabBar = true;
		windowClass.DockingAllowUnclassed = true;

		// If panel cannot float, prevent it from being undocked
		if (!m_CanFloat) {
			windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoUndocking;
		}

		ImGui::SetNextWindowClass(&windowClass);

		bool* pOpen = m_IsClosable ? &m_IsOpen : nullptr;
		ImGui::Begin(m_Name.c_str(), pOpen, windowFlags);

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window) {
			m_WindowID = window->ID;
			m_IsDocked = window->DockNode != nullptr && !window->DockNode->IsFloatingNode();

			if (window->DockNode) {
				ImGuiDockNodeFlags dockFlags = window->DockNode->LocalFlags;
				dockFlags &= ~ImGuiDockNodeFlags_AutoHideTabBar;

				if (!m_IsClosable) {
					dockFlags |= ImGuiDockNodeFlags_NoCloseButton;
				} else {
					dockFlags &= ~ImGuiDockNodeFlags_NoCloseButton;
				}

				window->DockNode->SetLocalFlags(dockFlags);
			}
		}


		m_IsFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		m_IsHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
	}

	void EditorPanel::EndPanel() {
		ImGui::End();
	}

	void EditorPanel::ShowCannotCloseTooltip() const {
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Cannot close the last panel");
		}
	}

	void EditorPanel::SetMinSize(float width, float height) {
		m_MinWidth = width;
		m_MinHeight = height;
	}

	ImVec2 EditorPanel::GetContentRegionAvail() const {
		return ImGui::GetContentRegionAvail();
	}

}
