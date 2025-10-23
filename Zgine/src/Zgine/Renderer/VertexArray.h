#pragma once

#include <memory>
#include "Zgine/Renderer/Buffer.h"

namespace Zgine {

	/**
	 * @brief Abstract base class for vertex arrays
	 * @details This class defines the interface for vertex array implementations,
	 *          managing vertex buffers and index buffers for rendering
	 */
	class VertexArray
	{
	public:
		/**
		 * @brief Destroy the VertexArray object
		 */
		virtual ~VertexArray() {}

		/**
		 * @brief Bind the vertex array
		 * @details Makes this vertex array active for rendering
		 */
		virtual void Bind() const = 0;
		
		/**
		 * @brief Unbind the vertex array
		 * @details Deactivates this vertex array
		 */
		virtual void Unbind() const = 0;

		/**
		 * @brief Add a vertex buffer to the vertex array
		 * @param vertexBuffer Reference-counted pointer to the vertex buffer
		 * @details Adds a vertex buffer to this vertex array
		 */
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		
		/**
		 * @brief Set the index buffer for the vertex array
		 * @param indexBuffer Reference-counted pointer to the index buffer
		 * @details Sets the index buffer for this vertex array
		 */
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		/**
		 * @brief Get all vertex buffers
		 * @return const std::vector<Ref<VertexBuffer>>& Reference to vector of vertex buffers
		 * @details Returns all vertex buffers attached to this vertex array
		 */
		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		
		/**
		 * @brief Get the index buffer
		 * @return const Ref<IndexBuffer>& Reference to the index buffer
		 * @details Returns the index buffer attached to this vertex array
		 */
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

		/**
		 * @brief Create a new vertex array
		 * @return VertexArray* Pointer to the created vertex array
		 * @details Creates a platform-specific vertex array implementation
		 */
		static VertexArray* Create();
	};
	
}