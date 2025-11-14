#include "Zgpch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

#if ZG_PLATFORM_WINDOWS
#include "Platform/Direct3D/Direct3DBuffer.h"
#endif

namespace Zgine {

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: ZG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); break;

			case RendererAPI::API::OpenGL: return new OpenGLVertexBuffer(vertices, size); break;
		}

		ZG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Zgine::IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: ZG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); break;

			case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, size); break;
		}

		ZG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	void BufferLayout::CalculateOffsetAndStride()
	{
		uint32_t offset = 0;
		m_Stride = 0;
		for (auto& element : m_Elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}

}

