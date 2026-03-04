#include <Zgine/Renderer/RHI/RendererAPI.h>
#include <Zgine/Core/Application/Application.h>
#include <Zgine/Gui/GuiLayer.h>
#include <Zgine/Core/OS/VFS.h>
#include <Zgine/Core/Log/Log.h>
#include <Zgine/Core/Base/Assert.h>
#include <Zgine/Core/Time/Timestep.h>

#include <GLFW/glfw3.h> // For glfwSwapBuffers in refresh callback

namespace Zgine {

    Application* Application::s_Instance = nullptr;

    Application::Application(const std::string& name)
    {
        ZGINE_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        // Initialize VFS (Virtual File System)
        if (!VFS::Initialize("Zgine")) {
            ZGINE_CORE_ERROR("Failed to initialize VFS!");
        }

        // Mount assets directory
        VFS::Mount("assets", nullptr, true);

        // Set write directory to current directory
        VFS::SetWriteDir(".");

        m_Window = Window::Create(WindowProps(name));
        m_Window->SetEventCallback(ZGINE_BIND_EVENT_FN(Application::OnEvent));

        m_GuiLayer = GuiLayer::Create();
        PushOverlay(m_GuiLayer.get());

        // Set refresh callback for continuous rendering during window resize
        // Note: Must NOT call glfwPollEvents inside this callback to avoid deadlock
        m_Window->SetRefreshCallback([this]() {
            if (!m_Minimized && m_GuiLayer) {
                float time = static_cast<float>(Timestep::GetTime());
                Timestep ts = time - m_LastFrameTime;
                m_LastFrameTime = time;

                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(ts);

                m_GuiLayer->Begin();
                for (Layer* layer : m_LayerStack)
                    layer->OnGuiRender();
                m_GuiLayer->End();

                // Only swap buffers, don't poll events (would cause deadlock)
                glfwSwapBuffers(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()));
            }
        });
    }

    Application::~Application()
    {
        // Shutdown VFS
        VFS::Shutdown();
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(ZGINE_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(ZGINE_BIND_EVENT_FN(Application::OnWindowResize));

        // 从后往前（从 Overlay 到 Layer）传递事件
        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if (e.Handled)
                break;
            (*it)->OnEvent(e);
        }
    }

    void Application::Run()
    {
        while (m_Running)
        {
            float time = static_cast<float>(Timestep::GetTime());
            m_Timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            if (!m_Minimized)
            {
                // 更新顺序：从前往后 (Layer -> Overlay)
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(m_Timestep);

                m_GuiLayer->Begin();
                for (Layer* layer : m_LayerStack)
                    layer->OnGuiRender();
                m_GuiLayer->End();
            }

            m_Window->OnUpdate();
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        ZGINE_UNUSED(e); // Suppress unused parameter warning
        m_Running = false;
        return true;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        // Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

        return false;
    }

}
