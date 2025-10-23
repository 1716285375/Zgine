#include "zgpch.h"
#include "WindowsWindow.h"

#include "Zgine/Events/ApplicationEvent.h"
#include "Zgine/Events/MouseEvent.h"
#include "Zgine/Events/KeyEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Zgine {
	
	static bool s_GLFWInitialized = false;
	
	/**
	 * @brief GLFW error callback function
	 * @param error The error code
	 * @param description The error description
	 * @details Logs GLFW errors using the engine's logging system
	 */
	static void GLFWErrorCallback(int error, const char* description)
	{
		ZG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	/**
	 * @brief Create a new window instance
	 * @param props The window properties
	 * @return Window* Pointer to the created window
	 * @details Factory method for creating platform-specific window implementations
	 */
	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	/**
	 * @brief Construct a new WindowsWindow object
	 * @param props The window properties
	 * @details Initializes the window with the given properties
	 */
	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	/**
	 * @brief Destroy the WindowsWindow object
	 * @details Properly cleans up GLFW window and context
	 */
	WindowsWindow::~WindowsWindow()
	{
		Shutdonw();
	}

	/**
	 * @brief Update the window (swap buffers, poll events)
	 * @details Called every frame to handle window updates
	 */
	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	/**
	 * @brief Enable or disable VSync
	 * @param enabled True to enable VSync, false to disable
	 * @details Sets GLFW swap interval and updates internal state
	 */
	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled) {
			glfwSwapInterval(1);
		}
		else {
			glfwSwapInterval(0);
		}

		m_Data.VSync = enabled;
	}

	/**
	 * @brief Check if VSync is enabled
	 * @return bool True if VSync is enabled
	 * @details Returns the current VSync state
	 */
	bool WindowsWindow::IsVSync() const
	{
		return false;
	}

	/**
	 * @brief Initialize the window
	 * @param props The window properties
	 * @details Creates GLFW window and sets up event callbacks
	 */
	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		ZG_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);
		
		if (!s_GLFWInitialized) {
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			ZG_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		m_Context = CreateScope<OpenGLContext>(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW callbacks
		// Window resize event callback
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		// Window close event callback
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		// Keyboard event callback
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int momds) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action) {
				case GLFW_PRESS: {
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT: {
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		// Character input event callback
		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		// Mouse button event callback
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE: {
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		// Mouse scroll event callback
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		// Mouse cursor position event callback
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	/**
	 * @brief Shutdown the window
	 * @details Destroys GLFW window and cleans up resources
	 */
	void WindowsWindow::Shutdonw()
	{
		glfwDestroyWindow(m_Window);
	}
}
