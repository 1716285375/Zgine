#include "zgpch.h"
#include "Application.h"

#include "Zgine/Log.h"

#include <glad/glad.h>


namespace Zgine {
	Application::Application()
	{
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
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			for (auto* layer : m_LayerStack) {
				layer->OnUpdate();
			}

			m_Window->OnUpdate();
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.PopLayer(layer);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
