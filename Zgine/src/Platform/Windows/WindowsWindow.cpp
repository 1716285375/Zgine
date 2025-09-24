#include "zgpch.h"
#include "WindowsWindow.h"

#include "Zgine/Events/ApplicationEvent.h"
#include "Zgine/Events/MouseEvent.h"
#include "Zgine/Events/KeyEvent.h"

#include "imgui.h"
#include <glad/glad.h>


namespace Zgine {
	
	static bool s_GLFWInitialized = false;
	static void GLFWErrorCallback(int error, const char* description)
	{
		ZG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdonw();
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

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

	bool WindowsWindow::IsVSync() const
	{
		return false;
	}

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
		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ZG_CORE_ASSERT(status, "Failed to initialized glad!");

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW callbacks
		// resize event
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		// close event
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		// keyboard event
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

			// map GLFW key to ImGui key
			ImGuiIO& io = ImGui::GetIO();
			ImGuiKey imgui_key = ImGuiKey_None;
			switch (key)
			{
			case GLFW_KEY_TAB:        imgui_key = ImGuiKey_Tab; break;
			case GLFW_KEY_LEFT:       imgui_key = ImGuiKey_LeftArrow; break;
			case GLFW_KEY_RIGHT:      imgui_key = ImGuiKey_RightArrow; break;
			case GLFW_KEY_UP:         imgui_key = ImGuiKey_UpArrow; break;
			case GLFW_KEY_DOWN:       imgui_key = ImGuiKey_DownArrow; break;
			case GLFW_KEY_PAGE_UP:    imgui_key = ImGuiKey_PageUp; break;
			case GLFW_KEY_PAGE_DOWN:  imgui_key = ImGuiKey_PageDown; break;
			case GLFW_KEY_HOME:       imgui_key = ImGuiKey_Home; break;
			case GLFW_KEY_END:        imgui_key = ImGuiKey_End; break;
			case GLFW_KEY_DELETE:     imgui_key = ImGuiKey_Delete; break;
			case GLFW_KEY_BACKSPACE:  imgui_key = ImGuiKey_Backspace; break;
			case GLFW_KEY_ENTER:      imgui_key = ImGuiKey_Enter; break;
			case GLFW_KEY_ESCAPE:     imgui_key = ImGuiKey_Escape; break;
			case GLFW_KEY_A:          imgui_key = ImGuiKey_A; break;
			case GLFW_KEY_C:          imgui_key = ImGuiKey_C; break;
			case GLFW_KEY_V:          imgui_key = ImGuiKey_V; break;
			case GLFW_KEY_X:          imgui_key = ImGuiKey_X; break;
			case GLFW_KEY_Y:          imgui_key = ImGuiKey_Y; break;
			case GLFW_KEY_Z:          imgui_key = ImGuiKey_Z; break;
			}
			if (imgui_key != ImGuiKey_None) {
				io.AddKeyEvent(imgui_key, action == GLFW_PRESS || action == GLFW_REPEAT);
			}


		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		// mouse button event
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

		// mouse scroll event
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		// cursor position event
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void WindowsWindow::Shutdonw()
	{
		glfwDestroyWindow(m_Window);
	}
}
