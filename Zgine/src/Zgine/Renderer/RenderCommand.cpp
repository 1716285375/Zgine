#include <zgpch.h>
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Zgine {
	/**
	 * @brief Static renderer API instance
	 * @details Initializes the renderer API with OpenGL implementation
	 */
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}