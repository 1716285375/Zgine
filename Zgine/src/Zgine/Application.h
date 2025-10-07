#pragma once

#include "Core.h"

#include "Window.h"	
#include "Zgine/LayerStack.h"
#include "Zgine/Events/Event.h"
#include "Zgine/Events/ApplicationEvent.h"

#include "Zgine/ImGui/ImGuiLayer.h"

#include "Zgine/Renderer/Shader.h"
#include "Zgine/Renderer/Buffer.h"



namespace Zgine {
	class ZG_API Application {
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		static Application* s_Instance;

		unsigned int m_VertexArray;

		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

