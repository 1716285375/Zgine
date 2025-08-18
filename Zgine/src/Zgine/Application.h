#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Zgine/Events/ApplicationEvent.h"
#include "Zgine/LayerStack.h"

#include "Window.h"	

namespace Zgine {
	class ZG_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

