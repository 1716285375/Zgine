#include "zgpch.h"
#include "Application.h"

#include <GLFW/glfw3.h>
#include "Input.h"
#include "Zgine/Log.h"

#include "Zgine/Layer.h"
#include "imgui.h"
#include "Zgine/Renderer/Renderer.h"

#include "Zgine/KeyCodes.h"

namespace Zgine {
	
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		ZG_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Scope<Window>(Window::Create());
		m_Window->SetEventCallback([this](Event& e) {OnEvent(e);});
		
		// Initialize renderer
		Renderer::Init();
		
		m_ImGuiLayer = CreateScope<ImGuiLayer>();
		PushOverlay(m_ImGuiLayer.get());
		
		// Call derived class initialization
		OnApplicationStart();
	}

	Application::~Application()
	{
		Renderer::Shutdown();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return OnWindowClose(e); });

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ) {
			(*--it)->OnEvent(e);
			if (e.Handled()) {
				break;
			}
		}
	}

	void Application::Run()
	{
		float lastFrameTime = 0.0f;
		
		while (m_Running) {
			float time = (float)glfwGetTime(); // TODO: Platform::GetTime
			Timestep timestep = time - lastFrameTime;
			lastFrameTime = time;

			RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
			RenderCommand::Clear();

			for (auto* layer : m_LayerStack) {
				layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin();
			for (auto* layer : m_LayerStack) {
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->OnImGuiRender(); // Add this line to call ImGuiLayer's OnImGuiRender
			m_ImGuiLayer->End();
			
			m_Window->OnUpdate();
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.PopLayer(layer);
	}

	void Application::PopOverlay(Layer* overlay)
	{
		m_LayerStack.PopOverlay(overlay);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
