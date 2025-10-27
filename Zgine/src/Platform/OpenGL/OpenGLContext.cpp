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

		// Verify OpenGL version is 4.3 or higher
		GLint major = 0, minor = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

		ZG_CORE_INFO("OpenGL Info:");
		ZG_CORE_INFO("  Vendor:   {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		ZG_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		ZG_CORE_INFO("  Version:  {0}.{1}", major, minor);

		// Enforce minimum OpenGL version 4.3
		if (major < 4 || (major == 4 && minor < 3)) {
			ZG_CORE_ERROR("OpenGL 4.3+ is required! Current version: {}.{}", major, minor);
			ZG_CORE_ASSERT(false, "OpenGL version too old! Minimum required: 4.3");
		}

		// Print OpenGL Shading Language version
		ZG_CORE_INFO("  GLSL:    {0}", reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

		// Enable depth testing for 3D rendering
		glEnable(GL_DEPTH_TEST);
		
		// Enable blending for transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Enable culling for performance
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

#ifdef ZG_DEBUG
		// Enable OpenGL debug output if available
		if (glDebugMessageCallback) {
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			
			// Set up debug callback
			glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
			                         GLsizei length, const GLchar* message, const void* userParam) {
				(void)source; (void)type; (void)id; (void)userParam;
				
				if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
				
				// Ignore texture state usage warnings about undefined base level
				// This warning occurs when texture image units are bound to texture object 0 (null texture)
				// We handle this by ensuring all texture slots are bound to valid textures
				std::string msg(message, length);
				if (msg.find("Texture state usage warning") != std::string::npos &&
				    msg.find("does not have a defined base level") != std::string::npos) {
					return;
				}
				
				if (type == GL_DEBUG_TYPE_ERROR) {
					ZG_CORE_ERROR("OpenGL Error [{}]: {}", severity, message);
				} else {
					ZG_CORE_WARN("OpenGL Warning [{}]: {}", severity, message);
				}
			}, nullptr);
			
			// Enable all severity levels
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_TRUE);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_TRUE);
			
			ZG_CORE_INFO("OpenGL Debug Output: ENABLED");
		}
#endif
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
