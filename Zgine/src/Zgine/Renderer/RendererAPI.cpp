#include "zgpch.h"
#include "RendererAPI.h"

namespace Zgine {
	
	/**
	 * @brief Static API instance initialization
	 * @details Sets the default rendering API to OpenGL
	 */
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
}