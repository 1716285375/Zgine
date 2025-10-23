#include "zgpch.h"
#include "OpenGLContext.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

namespace Zgine {

	/**
	 * @brief Construct a new OpenGLContext object
	 * @param windowHandle Pointer to the GLFW window handle
	 * @details Creates an OpenGL context for the specified window
	 */
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		ZG_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	/**
	 * @brief Destroy the OpenGLContext object
	 * @details Properly cleans up OpenGL context resources
	 */
	OpenGLContext::~OpenGLContext()
	{
		// OpenGL context cleanup is handled by GLFW
	}

	/**
	 * @brief Initialize the graphics context
	 * @details Sets up the OpenGL context and prepares it for rendering
	 */
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ZG_CORE_ASSERT(status, "Failed to initialize Glad!");

		ZG_CORE_INFO("OpenGL Info:");
		ZG_CORE_INFO("  Vendor:   {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		ZG_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		ZG_CORE_INFO("  Version:  {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

		// Enable depth testing for 3D rendering
		glEnable(GL_DEPTH_TEST);
		
		// Enable blending for transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	/**
	 * @brief Swap the front and back buffers
	 * @details Swaps the rendering buffers to display the rendered frame
	 */
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}
