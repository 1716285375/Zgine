#include "zgpch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

#if ZG_PLATFORM_WINDOWS
#include "Platform/Direct3D/Direct3DBuffer.h"
#endif

namespace Zgine {

	/**
	 * @brief Create a new vertex buffer
	 * @param vertices Pointer to vertex data
	 * @param size Size of the data in bytes
	 * @return VertexBuffer* Pointer to the created vertex buffer
	 * @details Creates a platform-specific vertex buffer implementation
	 */
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

	/**
	 * @brief Create a new index buffer
	 * @param indices Pointer to index data
	 * @param count Number of indices
	 * @return IndexBuffer* Pointer to the created index buffer
	 * @details Creates a platform-specific index buffer implementation
	 */
	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: ZG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); break;

			case RendererAPI::API::OpenGL: return new OpenGLIndexBuffer(indices, count); break;
		}

		ZG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	/**
	 * @brief Calculate offsets and stride for all elements
	 * @details Computes the offset for each element and the total stride
	 */
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

