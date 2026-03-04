#pragma once

#include <Zgine/Gui/GuiLayer.h>

struct nk_context;
struct nk_glfw;

namespace Zgine {

    class NuklearLayer : public GuiLayer {
    public:
        NuklearLayer();
        ~NuklearLayer();

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
        ::nk_context* m_Context = nullptr;
        std::unique_ptr<::nk_glfw> m_NkGlfw; // Pimpl to avoid including backend header here if possible, or just struct
    };
}
