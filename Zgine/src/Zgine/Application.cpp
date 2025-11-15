#include "Zgpch.h"
#include "Application.h"
#include "Input.h"
#include "Zgine/Log.h"

#include "Zgine/Core/Timestep.h"
#include "Zgine/Layer.h"
#include "imgui.h"
#include "Zgine/Renderer/Renderer.h"

#include <GLFW/glfw3.h>


namespace Zgine {
	
	Application* Application::s_Instance = nullptr;



	Application::Application()
	{
		ZG_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		//m_Window->(ZG_BIND_EVENT_FN(Application::OnEvent));
		// TODO: modern c++ use lambda
		m_Window->SetEventCallback([this](Event& e) {OnEvent(e);});

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{

	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		//dispatcher.Dispatch<WindowCloseEvent>(ZG_BIND_EVENT_FN(Application::OnWindowClose));
		// TODO: modern c++ use lambda
		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return OnWindowClose(e); });

		//ZG_CORE_TRACE("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ) {
			(*--it)->OnEvent(e);
			if (e.Handled()) {
				break;
			}
		}
	}

	void Application::Run()
	{
		while (m_Running) {

			float time = static_cast<float>(glfwGetTime());
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			for (auto* layer : m_LayerStack) {
				layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin();
			for (auto* layer : m_LayerStack) {
				layer->OnImGuiRender();
			}
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
