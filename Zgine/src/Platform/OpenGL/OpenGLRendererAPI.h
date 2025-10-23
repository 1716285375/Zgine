#pragma once

#include "Zgine/Renderer/RendererAPI.h"

namespace Zgine {

	/**
	 * @brief OpenGL implementation of the RendererAPI
	 * @details This class provides OpenGL-specific implementations of the rendering API
	 *          interface, handling OpenGL rendering operations
	 */
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		/**
		 * @brief Set the clear color for the renderer
		 * @param color The RGBA color to use for clearing the screen
		 * @details Sets the background color that will be used when clearing the screen
		 */
		virtual void SetClearColor(const glm::vec4& color) override;
		
		/**
		 * @brief Clear the screen
		 * @details Clears the color and depth buffers with the current clear color
		 */
		virtual void Clear() override;

		/**
		 * @brief Draw indexed geometry
		 * @param vertexArray The vertex array containing the geometry to draw
		 * @details Renders the geometry defined by the vertex array using indexed drawing
		 */
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;

		/**
		 * @brief Draw indexed geometry with specified count
		 * @param vertexArray The vertex array containing the geometry to draw
		 * @param indexCount The number of indices to draw
		 * @details Renders the geometry defined by the vertex array using indexed drawing with specified count
		 */
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) override;
	};

}