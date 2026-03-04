#include <Zgine/Editor/Manager/EditorPanelManager.h>
#include <Zgine/Editor/Panels/EditorPanel.h>

#include <algorithm>


namespace Zgine {

    EditorPanelManager::EditorPanelManager() = default;

    EditorPanelManager::~EditorPanelManager() = default;

    void EditorPanelManager::RemovePanel(const std::shared_ptr<EditorPanel>& panel) {
        auto it = std::find(m_Panels.begin(), m_Panels.end(), panel);
        if (it != m_Panels.end()) {
            (*it)->OnDetach();
            m_Panels.erase(it);
        }
    }

    void EditorPanelManager::OnUpdate(float deltaTime) {
        UpdatePanelStates();
        
        for (auto& panel : m_Panels) {
            if (panel->IsOpen()) {
                panel->OnUpdate(deltaTime);
            }
        }
    }

    void EditorPanelManager::OnGuiRender() {
        for (auto& panel : m_Panels) {
            if (panel->IsOpen()) {
                panel->OnGuiRender();
            }
        }
    }

    void EditorPanelManager::RenderPanelMenuItems() {
        if (ImGui::BeginMenu("Panels")) {
            for (auto& panel : m_Panels) {
                bool isOpen = panel->IsOpen();
                if (ImGui::MenuItem(panel->GetName().c_str(), nullptr, &isOpen)) {
                    panel->SetOpen(isOpen);
                }
            }
            ImGui::EndMenu();
        }
    }

    void EditorPanelManager::Clear() {
        for (auto& panel : m_Panels) {
            panel->OnDetach();
        }
        m_Panels.clear();
    }

    const std::vector<std::shared_ptr<EditorPanel>>& EditorPanelManager::GetPanels() const {
        return m_Panels;
    }

    std::shared_ptr<EditorPanel> EditorPanelManager::FindPanel(const std::string& name) const {
        auto it = std::find_if(m_Panels.begin(), m_Panels.end(),
        [&name](const std::shared_ptr<EditorPanel>& panel) {
            return panel->GetName() == name;
        });
        return (it != m_Panels.end()) ? *it : nullptr;
    }

    size_t EditorPanelManager::GetOpenPanelCount() const {
        return static_cast<size_t>(std::count_if(m_Panels.begin(), m_Panels.end(),
            [](const std::shared_ptr<EditorPanel>& panel) { return panel->IsOpen(); }));
    }

    size_t EditorPanelManager::GetDockedPanelCount() const {
        return static_cast<size_t>(std::count_if(m_Panels.begin(), m_Panels.end(),
            [](const std::shared_ptr<EditorPanel>& panel) {
                return panel->IsOpen() && panel->IsDocked();
            }));
    }

    void EditorPanelManager::SetEnforceLastPanelDocked(bool enforce) {
        m_EnforceLastPanelDocked = enforce;
    }

    bool EditorPanelManager::IsEnforceLastPanelDocked() const {
        return m_EnforceLastPanelDocked;
    }

    void EditorPanelManager::SetEnforceMinimumOnePanel(bool enforce) {
        m_EnforceMinimumOnePanel = enforce;
    }

    bool EditorPanelManager::IsEnforceMinimumOnePanel() const {
        return m_EnforceMinimumOnePanel;
    }

    void EditorPanelManager::UpdatePanelStates() {
        size_t openCount = GetOpenPanelCount();
        size_t dockedCount = GetDockedPanelCount();

        if (m_EnforceMinimumOnePanel && openCount == 1) {
            for (auto& panel : m_Panels) {
                if (panel->IsOpen()) {
                    panel->SetClosable(false);
                }
            }
        } else {
            for (auto& panel : m_Panels) {
                if (!panel->IsClosable()) {
                    panel->SetClosable(true);
                }
            }
        }

        if (m_EnforceLastPanelDocked && dockedCount == 1) {
            for (auto& panel : m_Panels) {
                if (panel->IsOpen() && panel->IsDocked()) {
                    panel->SetFloatable(false);
                }
            }
        } else {
            for (auto& panel : m_Panels) {
                if (!panel->IsFloatable()) {
                    panel->SetFloatable(true);
                }
            }
        }
    }
}
