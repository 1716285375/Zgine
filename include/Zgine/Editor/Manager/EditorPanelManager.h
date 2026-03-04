#pragma once

#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <utility>
#include <imgui.h>
#include <imgui_internal.h>

namespace Zgine {

    class EditorPanel;

    class EditorPanelManager {
    public:
        EditorPanelManager();
        ~EditorPanelManager();

        template<typename T, typename... Args>
        std::shared_ptr<T> AddPanel(Args&&... args){
            static_assert(std::is_base_of_v<EditorPanel, T>, "T must derive from EditorPanel");
            auto panel = std::make_shared<T>(std::forward<Args>(args)...);
            m_Panels.push_back(panel);
            panel->OnAttach();
            return panel;
        }

        void RemovePanel(const std::shared_ptr<EditorPanel>& panel);

        void OnUpdate(float deltaTime);

        void OnGuiRender();

        void RenderPanelMenuItems();

        void Clear();

        const std::vector<std::shared_ptr<EditorPanel>>& GetPanels() const;

        std::shared_ptr<EditorPanel> FindPanel(const std::string& name) const;

        size_t GetOpenPanelCount() const;

        size_t GetDockedPanelCount() const;

        void SetEnforceLastPanelDocked(bool enforce);

        bool IsEnforceLastPanelDocked() const;

        void SetEnforceMinimumOnePanel(bool enforce);

        bool IsEnforceMinimumOnePanel() const;

    private:
        void UpdatePanelStates();

        std::vector<std::shared_ptr<EditorPanel>> m_Panels;
        bool m_EnforceLastPanelDocked = true;
        bool m_EnforceMinimumOnePanel = true;
    };
}
