#include "zgpch.h"
#include "Buffer.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

#if ZG_PLATFORM_WINDOWS
#include "Platform/Direct3D/Direct3DBuffer.h"
#endif

namespace Zgine {

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: ZG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			case RendererAPI::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}

		ZG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Zgine::IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: ZG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			case RendererAPI::OpenGL: return new OpenGLIndexBuffer(indices, size);
		}

		ZG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}

