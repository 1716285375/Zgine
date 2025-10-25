#include "zgpch.h"
#include "Application.h"

#include <GLFW/glfw3.h>
#include "Input.h"
#include "Zgine/Logging/Log.h"
#include "Zgine/Layer.h"
#include "imgui.h"
#include "Zgine/Renderer/Renderer.h"
#include "Zgine/KeyCodes.h"

namespace Zgine {
	
	Application* Application::s_Instance = nullptr;
}

// Global application shutdown flag (outside namespace for global access)
bool g_ApplicationShuttingDown = false;

namespace Zgine {

	/**
	 * @brief Construct a new Application object
	 * @details Initializes the application singleton, creates the main window,
	 *          initializes the renderer, and sets up the ImGui layer
	 */
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
		
		// Don't call OnApplicationStart here - virtual function calls in constructor don't work properly
	}

	/**
	 * @brief Destroy the Application object
	 * @details Safely shuts down the application and cleans up resources
	 */
	Application::~Application()
	{
		ZG_CORE_INFO("Application destructor called");
		
		// Set global shutdown flag before shutting down renderer
		extern bool g_ApplicationShuttingDown;
		g_ApplicationShuttingDown = true;
		
		// Stop the main loop if it's still running
		m_Running = false;
		
		// Shutdown ImGui layer first to prevent rendering calls
		if (m_ImGuiLayer)
		{
			ZG_CORE_INFO("Shutting down ImGui layer");
			// Remove ImGuiLayer from layer stack before destroying it
			PopOverlay(m_ImGuiLayer.get());
			m_ImGuiLayer->OnDetach();
			m_ImGuiLayer.reset();
		}
		
		// Layer stack will be cleared by its destructor
		
		// Now shutdown renderer
		ZG_CORE_INFO("Shutting down renderer");
		Renderer::Shutdown();
		
		ZG_CORE_INFO("Application destructor completed");
	}

	/**
	 * @brief Handle application events
	 * @param e The event to handle
	 * @details Dispatches events to the appropriate handlers and layers
	 */
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

	/**
	 * @brief Run the main application loop
	 * @details Starts the main game loop, handles updates, rendering, and events
	 */
	void Application::Run()
	{
		// Call OnApplicationStart after Application is fully constructed
		static bool firstRun = true;
		if (firstRun)
		{
			ZG_CORE_INFO("Application::Run calling OnApplicationStart");
			OnApplicationStart();
			ZG_CORE_INFO("Application::Run OnApplicationStart completed");
			firstRun = false;
		}
		
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
			// ImGuiLayer is already in LayerStack as overlay, so its OnImGuiRender is called above
			m_ImGuiLayer->End();
			
			m_Window->OnUpdate();
		}
	}

	/**
	 * @brief Push a layer onto the layer stack
	 * @param layer Pointer to the layer to add
	 * @details Adds the layer to the stack and calls its OnAttach method
	 */
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	/**
	 * @brief Push an overlay onto the layer stack
	 * @param overlay Pointer to the overlay to add
	 * @details Adds the overlay to the stack and calls its OnAttach method
	 */
	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	/**
	 * @brief Pop a layer from the layer stack
	 * @param layer Pointer to the layer to remove
	 * @details Removes the layer from the stack
	 */
	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.PopLayer(layer);
	}

	/**
	 * @brief Pop an overlay from the layer stack
	 * @param overlay Pointer to the overlay to remove
	 * @details Removes the overlay from the stack
	 */
	void Application::PopOverlay(Layer* overlay)
	{
		m_LayerStack.PopOverlay(overlay);
	}

	/**
	 * @brief Handle window close events
	 * @param e The window close event
	 * @return bool True if the event was handled
	 * @details Sets the running flag to false to exit the main loop
	 */
	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
