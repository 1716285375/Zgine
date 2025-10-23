#pragma once

#include "Zgine/Renderer/Buffer.h"

namespace Zgine {

	/**
	 * @brief OpenGL implementation of vertex buffers
	 * @details This class provides OpenGL-specific implementation of vertex buffers,
	 *          handling vertex data storage and layout management
	 */
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		/**
		 * @brief Construct a new OpenGLVertexBuffer object
		 * @param vertices Pointer to vertex data
		 * @param size Size of the data in bytes
		 * @details Creates an OpenGL vertex buffer with the specified data
		 */
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		
		/**
		 * @brief Destroy the OpenGLVertexBuffer object
		 * @details Properly cleans up OpenGL vertex buffer resources
		 */
		virtual ~OpenGLVertexBuffer();

		/**
		 * @brief Bind the vertex buffer
		 * @details Makes this OpenGL vertex buffer active for rendering
		 */
		virtual void Bind() const override;
		
		/**
		 * @brief Unbind the vertex buffer
		 * @details Deactivates this OpenGL vertex buffer
		 */
		virtual void Unbind() const override;

		/**
		 * @brief Get the buffer layout
		 * @return const BufferLayout& Reference to the buffer layout
		 * @details Returns the layout of vertex attributes in this buffer
		 */
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		
		/**
		 * @brief Set the buffer layout
		 * @param layout The buffer layout to set
		 * @details Sets the layout of vertex attributes for this buffer
		 */
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

		/**
		 * @brief Set vertex data
		 * @param data Pointer to the vertex data
		 * @param size Size of the data in bytes
		 * @details Uploads vertex data to the OpenGL buffer
		 */
		virtual void SetData(const void* data, uint32_t size) override;

	private:
		uint32_t m_RendererID;    ///< OpenGL vertex buffer ID
		BufferLayout m_Layout;    ///< Buffer layout for vertex attributes
	};

	/**
	 * @brief OpenGL implementation of index buffers
	 * @details This class provides OpenGL-specific implementation of index buffers,
	 *          handling index data for indexed rendering
	 */
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		/**
		 * @brief Construct a new OpenGLIndexBuffer object
		 * @param indices Pointer to index data
		 * @param count Number of indices
		 * @details Creates an OpenGL index buffer with the specified indices
		 */
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		
		/**
		 * @brief Destroy the OpenGLIndexBuffer object
		 * @details Properly cleans up OpenGL index buffer resources
		 */
		virtual ~OpenGLIndexBuffer();

		/**
		 * @brief Bind the index buffer
		 * @details Makes this OpenGL index buffer active for rendering
		 */
		virtual void Bind() const override;
		
		/**
		 * @brief Unbind the index buffer
		 * @details Deactivates this OpenGL index buffer
		 */
		virtual void Unbind() const override;

		/**
		 * @brief Get the number of indices
		 * @return uint32_t The number of indices
		 * @details Returns the count of indices in this buffer
		 */
		virtual uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_RendererID;    ///< OpenGL index buffer ID
		uint32_t m_Count;         ///< Number of indices in the buffer
	};

}