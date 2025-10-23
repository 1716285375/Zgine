#pragma once

#include "Zgine/Renderer/VertexArray.h"

namespace Zgine {

	/**
	 * @brief OpenGL implementation of vertex arrays
	 * @details This class provides OpenGL-specific implementation of vertex arrays,
	 *          managing vertex buffers and index buffers for rendering
	 */
	class OpenGLVertexArray : public VertexArray
	{
	public:
		/**
		 * @brief Construct a new OpenGLVertexArray object
		 * @details Creates an OpenGL vertex array object
		 */
		OpenGLVertexArray();
		
		/**
		 * @brief Destroy the OpenGLVertexArray object
		 * @details Properly cleans up OpenGL vertex array resources
		 */
		virtual ~OpenGLVertexArray();

		/**
		 * @brief Bind the vertex array
		 * @details Makes this OpenGL vertex array active for rendering
		 */
		virtual void Bind() const override;
		
		/**
		 * @brief Unbind the vertex array
		 * @details Deactivates this OpenGL vertex array
		 */
		virtual void Unbind() const override;

		/**
		 * @brief Add a vertex buffer to the vertex array
		 * @param vertexBuffer Reference-counted pointer to the vertex buffer
		 * @details Adds a vertex buffer to this OpenGL vertex array
		 */
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		
		/**
		 * @brief Set the index buffer for the vertex array
		 * @param indexBuffer Reference-counted pointer to the index buffer
		 * @details Sets the index buffer for this OpenGL vertex array
		 */
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		/**
		 * @brief Get all vertex buffers
		 * @return const std::vector<Ref<VertexBuffer>>& Reference to vector of vertex buffers
		 * @details Returns all vertex buffers attached to this vertex array
		 */
		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		
		/**
		 * @brief Get the index buffer
		 * @return const Ref<IndexBuffer>& Reference to the index buffer
		 * @details Returns the index buffer attached to this vertex array
		 */
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

	private:
		uint32_t m_RendererID;                           ///< OpenGL vertex array object ID
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;  ///< Vector of vertex buffers
		Ref<IndexBuffer> m_IndexBuffer;                  ///< Index buffer reference
	};

}