#include "zgpch.h"
#include "Application.h"

#include "Zgine/Log.h"

#include <GLFW/glfw3.h>


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

		ZG_CORE_TRACE(e);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	void Application::Run()
	{
		while (m_Running) {
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}
}
