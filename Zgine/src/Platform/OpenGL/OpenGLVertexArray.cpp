#include "zgpch.h"
#include "OpenGLVertexArray.h"
#include <glad/glad.h>

namespace Zgine {

	/**
	 * @brief Convert shader data type to OpenGL base type
	 * @param type The shader data type
	 * @return GLenum The corresponding OpenGL type
	 * @details Converts Zgine shader data types to OpenGL types for vertex attributes
	 */
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Zgine::ShaderDataType::Float:          return GL_FLOAT;
		case Zgine::ShaderDataType::Float2:         return GL_FLOAT;
		case Zgine::ShaderDataType::Float3:         return GL_FLOAT;
		case Zgine::ShaderDataType::Float4:         return GL_FLOAT;
		case Zgine::ShaderDataType::Mat3:           return GL_FLOAT;
		case Zgine::ShaderDataType::Mat4:           return GL_FLOAT;
		case Zgine::ShaderDataType::Int:            return GL_INT;
		case Zgine::ShaderDataType::Int2:           return GL_INT;
		case Zgine::ShaderDataType::Int3:           return GL_INT;
		case Zgine::ShaderDataType::Int4:           return GL_INT;
		case Zgine::ShaderDataType::Bool:           return GL_BOOL;
		}

		ZG_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	/**
	 * @brief Construct a new OpenGLVertexArray object
	 * @details Creates an OpenGL vertex array object
	 */
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	/**
	 * @brief Destroy the OpenGLVertexArray object
	 * @details Properly cleans up OpenGL vertex array resources
	 */
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	/**
	 * @brief Bind the vertex array
	 * @details Makes this OpenGL vertex array active for rendering
	 */
	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	/**
	 * @brief Unbind the vertex array
	 * @details Deactivates this OpenGL vertex array
	 */
	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	/**
	 * @brief Add a vertex buffer to the vertex array
	 * @param vertexBuffer Reference-counted pointer to the vertex buffer
	 * @details Adds a vertex buffer to this OpenGL vertex array
	 */
	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		ZG_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				reinterpret_cast<const void*>(static_cast<uintptr_t>(element.Offset))
			);
			++index;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	/**
	 * @brief Set the index buffer for the vertex array
	 * @param indexBuffer Reference-counted pointer to the index buffer
	 * @details Sets the index buffer for this OpenGL vertex array
	 */
	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}