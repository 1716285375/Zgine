#pragma once

#include "Zgine/Window.h"
#include <GLFW/glfw3.h>
#include "Zgine/Renderer/GraphicsContext.h"

namespace Zgine {

	/**
	 * @brief Windows platform specific window implementation
	 * @details This class implements the Window interface using GLFW
	 *          for Windows platform window management
	 */
	class WindowsWindow : public Window
	{
	public:
		/**
		 * @brief Construct a new WindowsWindow object
		 * @param props The window properties
		 */
		WindowsWindow(const WindowProps& props);
		
		/**
		 * @brief Destroy the WindowsWindow object
		 * @details Properly cleans up GLFW window and context
		 */
		virtual ~WindowsWindow();

		/**
		 * @brief Update the window (swap buffers, poll events)
		 * @details Called every frame to handle window updates
		 */
		void OnUpdate() override;

		/**
		 * @brief Get the window width
		 * @return unsigned int The window width in pixels
		 */
		inline unsigned int GetWidth() const override { return m_Data.Width; }
		
		/**
		 * @brief Get the window height
		 * @return unsigned int The window height in pixels
		 */
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		/**
		 * @brief Set the event callback function
		 * @param callback The function to call when events occur
		 */
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		
		/**
		 * @brief Enable or disable VSync
		 * @param enabled True to enable VSync, false to disable
		 */
		void SetVSync(bool enabled) override;
		
		/**
		 * @brief Check if VSync is enabled
		 * @return bool True if VSync is enabled
		 */
		bool IsVSync() const override;

		/**
		 * @brief Get the native window handle
		 * @return void* Pointer to the GLFW window handle
		 */
		inline virtual void* GetNativeWindow() const { return m_Window; }

	private:
		/**
		 * @brief Initialize the window
		 * @param props The window properties
		 * @details Creates GLFW window and sets up event callbacks
		 */
		virtual void Init(const WindowProps& props);
		
		/**
		 * @brief Shutdown the window
		 * @details Destroys GLFW window and cleans up resources
		 */
		virtual void Shutdonw();
		
	private:
		GLFWwindow* m_Window;                    ///< GLFW window handle
		Scope<GraphicsContext> m_Context;        ///< Graphics context for rendering

		/**
		 * @brief Window data structure
		 * @details Contains window properties and event callback
		 */
		struct WindowData
		{
			std::string Title;                   ///< Window title
			unsigned int Width, Height;          ///< Window dimensions
			bool VSync;                          ///< VSync enabled state
			EventCallbackFn EventCallback;      ///< Event callback function
		};

		WindowData m_Data;                       ///< Window data instance
	};

}


