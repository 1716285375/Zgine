#pragma once

#include <Zgine/Platform/Window.h>

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

namespace Zgine {

    class GLFWWindow : public Window {
    public:
        GLFWWindow(const WindowProps& props);
        virtual ~GLFWWindow();

        void OnUpdate() override;
        void SwapBuffers() override;

        inline unsigned int GetWidth() const override { return m_Data.Width; }
        inline unsigned int GetHeight() const override { return m_Data.Height; }
        inline WindowGraphicsAPI GetGraphicsAPI() const override { return m_Data.GraphicsAPI; }

        // Window attributes
        inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
        void SetRefreshCallback(const RefreshCallbackFn& callback) override;
        
        bool IsRunning() const override;

        inline void* GetNativeWindow() const override { return m_Window; }

    private:
        virtual void Init(const WindowProps& props);
        virtual void Shutdown();
        void UpdateInputState();

        GLFWwindow* m_Window;

        struct WindowData {
            std::string Title;
            unsigned int Width, Height;
            WindowGraphicsAPI GraphicsAPI = WindowGraphicsAPI::OpenGL;
            EventCallbackFn EventCallback;
            RefreshCallbackFn RefreshCallback;
            float ScrollDelta = 0.0f;
        };

        WindowData m_Data;
    };

}
