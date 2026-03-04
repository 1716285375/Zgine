#pragma once

#include <string>
#include <memory>
#include <functional>
#include <Zgine/Core/Events/Event.h>

namespace Zgine {

    struct WindowProps {
        std::string Title;
        unsigned int Width;
        unsigned int Height;

        WindowProps(const std::string& title = "Zgine Engine",
                    unsigned int width = 1280,
                    unsigned int height = 720)
            : Title(title), Width(width), Height(height) {}
    };

    // Interface representing a desktop system based Window
    class Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>;
        using RefreshCallbackFn = std::function<void()>;

        virtual ~Window() = default;

        virtual void OnUpdate() = 0;

        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;

        // Window attributes
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual void SetRefreshCallback(const RefreshCallbackFn& callback) = 0;
        virtual bool IsRunning() const = 0;

        virtual void* GetNativeWindow() const = 0;

        static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());
    };

}
