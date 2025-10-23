#include <zgpch.h>
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include <glad/glad.h>

namespace Zgine {
	/**
	 * @brief Check for OpenGL errors and log them
	 * @details Checks for OpenGL errors and logs them with context information
	 */
	void RenderCommand::CheckOpenGLError(const std::string& context)
	{
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::string errorString;
			switch (error)
			{
				case GL_INVALID_ENUM: errorString = "GL_INVALID_ENUM"; break;
				case GL_INVALID_VALUE: errorString = "GL_INVALID_VALUE"; break;
				case GL_INVALID_OPERATION: errorString = "GL_INVALID_OPERATION"; break;
				case GL_OUT_OF_MEMORY: errorString = "GL_OUT_OF_MEMORY"; break;
				case GL_INVALID_FRAMEBUFFER_OPERATION: errorString = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
				default: errorString = "Unknown OpenGL Error (" + std::to_string(error) + ")"; break;
			}
			ZG_CORE_ERROR("OpenGL Error in {}: {}", context, errorString);
		}
	}

	/**
	 * @brief Static renderer API instance
	 * @details Initializes the renderer API with OpenGL implementation
	 */
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}