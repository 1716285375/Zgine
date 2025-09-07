#include "zgpch.h"
#include "Application.h"
#include "Input.h"
#include "Zgine/Log.h"

#include <glad/glad.h>


namespace Zgine {
	
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		ZG_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		//m_Window->(BIND_EVENT_FN(Application::OnEvent));
		// TODO: modern c++ use lambda
		m_Window->SetEventCallback([this](Event& e) {OnEvent(e);});
	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		//dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		// TODO: modern c++ use lambda
		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return OnWindowClose(e); });

		ZG_CORE_TRACE("{0}", e);

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
			glClearColor(0.878f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			for (auto* layer : m_LayerStack) {
				layer->OnUpdate();
			}

			auto [x, y] = Input::GetMousePosition();
			//ZG_CORE_TRACE("{0}, {1}", x, y);
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
