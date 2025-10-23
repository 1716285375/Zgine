#pragma once

#include "zgpch.h"
#include "Zgine/Core.h"
#include "Zgine/Events/Event.h"

namespace Zgine {
	
	/**
	 * @brief Window properties structure
	 * @details Contains the basic properties needed to create a window
	 */
	struct WindowProps
	{
		std::string Title;      ///< Window title
		unsigned int Width;     ///< Window width in pixels
		unsigned int Height;    ///< Window height in pixels

		/**
		 * @brief Construct a new WindowProps object
		 * @param title The window title
		 * @param width The window width
		 * @param height The window height
		 */
		WindowProps(const std::string& title = "Zgine",
					unsigned int width = 1280,
					unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{
			// WindowProps constructor implementation
		}
	};

	/**
	 * @brief Interface representing a desktop system based Window
	 * @details This abstract base class defines the interface for platform-specific
	 *          window implementations
	 */
	class ZG_API Window
	{
	public:
		/**
		 * @brief Event callback function type
		 * @details Function signature for handling window events
		 */
		using EventCallbackFn = std::function<void(Event&)>;

		/**
		 * @brief Destroy the Window object
		 */
		virtual ~Window() {}

		/**
		 * @brief Update the window (swap buffers, poll events)
		 * @details Called every frame to handle window updates
		 */
		virtual void OnUpdate() = 0;

		/**
		 * @brief Get the window width
		 * @return unsigned int The window width in pixels
		 */
		virtual unsigned int GetWidth() const = 0;
		
		/**
		 * @brief Get the window height
		 * @return unsigned int The window height in pixels
		 */
		virtual unsigned int GetHeight() const = 0;

		/**
		 * @brief Set the event callback function
		 * @param callback The function to call when events occur
		 */
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		
		/**
		 * @brief Enable or disable VSync
		 * @param enabled True to enable VSync, false to disable
		 */
		virtual void SetVSync(bool enabled) = 0;
		
		/**
		 * @brief Check if VSync is enabled
		 * @return bool True if VSync is enabled
		 */
		virtual bool IsVSync() const = 0;

		/**
		 * @brief Get the native window handle
		 * @return void* Pointer to the native window handle
		 * @details Platform-specific window handle (HWND on Windows, etc.)
		 */
		virtual void* GetNativeWindow() const = 0;

		/**
		 * @brief Create a new window instance
		 * @param props The window properties
		 * @return Window* Pointer to the created window
		 * @details Factory method for creating platform-specific window implementations
		 */
		static Window* Create(const WindowProps& props = WindowProps());
	};
}