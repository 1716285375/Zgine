#pragma once

#include <Zgine/Gui/GuiLayer.h>

namespace Zgine {

    class ImGuiLayer : public GuiLayer {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnEvent(Event& e) override;

        virtual void Begin() override;
        virtual void End() override;

        virtual void BlockEvents(bool block) override { m_BlockEvents = block; }
        virtual void EnableDocking(bool enable) override;
        virtual void EnableViewports(bool enable) override;

        // Theme Management
        virtual void SetTheme(GuiTheme theme) override;
        virtual void SetCustomTheme() override;

        // Font Management
        virtual void LoadFont(const GuiFontConfig& config) override;
        virtual void SetGlobalFontScale(float scale) override;

        // Interaction Queries
        virtual bool IsMouseCaptured() const override;
        virtual bool IsKeyboardCaptured() const override;

    private:
        bool m_BlockEvents = true;
        float m_Time = 0.0f;
    };

}
