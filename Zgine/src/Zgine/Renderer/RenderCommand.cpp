#include <zgpch.h>
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Zgine {
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}