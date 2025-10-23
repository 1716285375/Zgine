#pragma once

#include "zgpch.h"
#include "Zgine/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Zgine {

	/**
	 * @brief OpenGL implementation of graphics context
	 * @details This class provides OpenGL-specific implementation of graphics context,
	 *          handling OpenGL context creation and buffer swapping
	 */
	class OpenGLContext : public GraphicsContext
	{
	public:
		/**
		 * @brief Construct a new OpenGLContext object
		 * @param windowHandle Pointer to the GLFW window handle
		 * @details Creates an OpenGL context for the specified window
		 */
		OpenGLContext(GLFWwindow* windowHandle);
		
		/**
		 * @brief Destroy the OpenGLContext object
		 * @details Properly cleans up OpenGL context resources
		 */
		virtual ~OpenGLContext();

		/**
		 * @brief Initialize the graphics context
		 * @details Sets up the OpenGL context and prepares it for rendering
		 */
		virtual void Init() override;
		
		/**
		 * @brief Swap the front and back buffers
		 * @details Swaps the rendering buffers to display the rendered frame
		 */
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;  ///< Pointer to the GLFW window handle
	};

}

