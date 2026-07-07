#pragma once

#include <Zgine/Core/Foundation/Prerequisites.h>
#include <Zgine/Core/Events/Event.h>
#include <Zgine/Core/Events/ApplicationEvent.h>
#include <Zgine/Core/Application/LayerStack.h>
#include <Zgine/Platform/Window.h>
#include <Zgine/Core/Time/Timestep.h>
#include <Zgine/Core/Time/TimerManager.h>
#include <Zgine/Gui/GuiLayer.h>

namespace Zgine {

    class Application {
    public:
        Application(const std::string& name = "Zgine App");
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        inline Window& GetWindow() { return *m_Window; }
        inline float GetTime() const { return static_cast<float>(Timestep::GetTime()); }
        inline Timestep GetTimestep() const { return m_Timestep; }
        inline TimerManager& GetTimerManager() { return m_TimerManager; }

        inline static Application& Get() { return *s_Instance; }

    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
        bool m_Minimized = false;

        // CRITICAL: m_GuiLayer must be declared BEFORE m_LayerStack!
        // Members destruct in reverse declaration order.
        // m_LayerStack holds a raw pointer to GuiLayer, so it must destruct first.
        Ref<GuiLayer> m_GuiLayer;          // Destructs SECOND (deletes GuiLayer object)
        LayerStack m_LayerStack;            // Destructs FIRST (accesses GuiLayer via OnDetach)

        TimerManager m_TimerManager;

        float m_LastFrameTime = 0.0f;
        Timestep m_Timestep;

    private:
        static Application* s_Instance;
    };

    // 在客户端定义
    Application* CreateApplication();

}
