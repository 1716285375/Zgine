#include "zgpch.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include <glad/glad.h>

namespace Zgine {

	// ================================================================================================
	// OpenGLVertexBuffer Implementation
	// ================================================================================================

	/**
	 * @brief Construct a new OpenGLVertexBuffer object
	 * @param vertices Pointer to vertex data
	 * @param size Size of the data in bytes
	 * @details Creates an OpenGL vertex buffer with the specified data
	 */
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		// Use glGenBuffers for broader compatibility
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	/**
	 * @brief Destroy the OpenGLVertexBuffer object
	 * @details Properly cleans up OpenGL vertex buffer resources
	 */
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	/**
	 * @brief Bind the vertex buffer
	 * @details Makes this OpenGL vertex buffer active for rendering
	 */
	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	/**
	 * @brief Unbind the vertex buffer
	 * @details Deactivates this OpenGL vertex buffer
	 */
	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/**
	 * @brief Set vertex data
	 * @param data Pointer to the vertex data
	 * @param size Size of the data in bytes
	 * @details Uploads vertex data to the OpenGL buffer
	 */
	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		// Bind the buffer before updating
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	// ================================================================================================
	// OpenGLIndexBuffer Implementation
	// ================================================================================================

	/**
	 * @brief Construct a new OpenGLIndexBuffer object
	 * @param indices Pointer to index data
	 * @param count Number of indices
	 * @details Creates an OpenGL index buffer with the specified indices
	 */
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		// Use glGenBuffers for broader compatibility
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	/**
	 * @brief Destroy the OpenGLIndexBuffer object
	 * @details Properly cleans up OpenGL index buffer resources
	 */
	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	/**
	 * @brief Bind the index buffer
	 * @details Makes this OpenGL index buffer active for rendering
	 */
	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	/**
	 * @brief Unbind the index buffer
	 * @details Deactivates this OpenGL index buffer
	 */
	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}