#pragma once

#include "Core.h"
#include "Window.h"	
#include "Zgine/LayerStack.h"
#include "Zgine/Events/Event.h"
#include "Zgine/Events/ApplicationEvent.h"
#include "Zgine/ImGui/ImGuiLayer.h"

namespace Zgine {

	/**
	 * @brief Main application class managing the engine lifecycle
	 * @details This class serves as the central hub for the engine,
	 *          managing the window, layer stack, and main game loop
	 */
	class ZG_API Application {
	public:
		/**
		 * @brief Construct a new Application object
		 */
		Application();
		
		/**
		 * @brief Destroy the Application object
		 * @details Safely shuts down the application and cleans up resources
		 */
		virtual ~Application();

		/**
		 * @brief Run the main application loop
		 * @details Starts the main game loop and handles events
		 */
		void Run();

		/**
		 * @brief Handle application events
		 * @param e The event to handle
		 */
		void OnEvent(Event& e);

		/**
		 * @brief Push a layer onto the layer stack
		 * @param layer Pointer to the layer to add
		 */
		void PushLayer(Layer* layer);
		
		/**
		 * @brief Push an overlay onto the layer stack
		 * @param overlay Pointer to the overlay to add
		 */
		void PushOverlay(Layer* overlay);

		/**
		 * @brief Pop a layer from the layer stack
		 * @param layer Pointer to the layer to remove
		 */
		void PopLayer(Layer* layer);
		
		/**
		 * @brief Pop an overlay from the layer stack
		 * @param overlay Pointer to the overlay to remove
		 */
		void PopOverlay(Layer* overlay);

		/**
		 * @brief Get the main window
		 * @return Window& Reference to the main window
		 */
		inline Window& GetWindow() { return *m_Window; }
		
		/**
		 * @brief Get the singleton application instance
		 * @return Application& Reference to the application instance
		 */
		inline static Application& Get() { return *s_Instance; }

	protected:
		/**
		 * @brief Called when the application starts
		 * @details Override this method to perform custom initialization
		 */
		virtual void OnApplicationStart() {}

	private:
		/**
		 * @brief Handle window close events
		 * @param e The window close event
		 * @return bool True if the event was handled
		 */
		bool OnWindowClose(WindowCloseEvent& e);

		Scope<Window> m_Window;           ///< Main application window
		Scope<ImGuiLayer> m_ImGuiLayer;  ///< ImGui layer for debugging UI
		bool m_Running = true;            ///< Whether the application is running
		LayerStack m_LayerStack;          ///< Stack of application layers
		static Application* s_Instance;   ///< Singleton application instance
	};

	/**
	 * @brief Factory function to create the application instance
	 * @details This function must be implemented by the client application
	 * @return Application* Pointer to the created application instance
	 */
	Application* CreateApplication();

}

