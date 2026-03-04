#pragma once

#include <Zgine/Core/Layers/Layer.h>
#include <Zgine/Core/Base/Prerequisites.h>

namespace Zgine {

    enum class GuiTheme {
        Dark, Light, Classic, Custom
    };

    struct GuiFontConfig {
        std::string Name;
        std::string FilePath;
        float Size;
        bool IsDefault = false;
    };

    class GuiLayer : public Layer {
    public:
        GuiLayer(const std::string& name = "GuiLayer") : Layer(name) {}
        virtual ~GuiLayer() = default;

        // Lifecycle
        virtual void Begin() = 0;
        virtual void End() = 0;

        // Configuration
        virtual void BlockEvents(bool block) = 0;
        virtual void EnableDocking(bool enable) = 0;
        virtual void EnableViewports(bool enable) = 0;

        // Theme Management
        virtual void SetTheme(GuiTheme theme) = 0;
        virtual void SetCustomTheme() = 0; // Hooks for user-defined styling

        // Font Management
        virtual void LoadFont(const GuiFontConfig& config) = 0;
        virtual void SetGlobalFontScale(float scale) = 0;

        // Interaction Queries
        virtual bool IsMouseCaptured() const = 0;
        virtual bool IsKeyboardCaptured() const = 0;

        static Ref<GuiLayer> Create();
    };

}
