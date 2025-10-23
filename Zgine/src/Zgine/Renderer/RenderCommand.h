#pragma once

#include "RendererAPI.h"

namespace Zgine {

	/**
	 * @brief High-level rendering command interface
	 * @details This class provides a high-level interface for rendering commands,
	 *          abstracting away the underlying rendering API implementation
	 */
	class RenderCommand
	{
	public:
		/**
		 * @brief Set the clear color for the renderer
		 * @param color The RGBA color to use for clearing the screen
		 * @details Sets the background color that will be used when clearing the screen
		 */
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		/**
		 * @brief Clear the screen
		 * @details Clears the color and depth buffers with the current clear color
		 */
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		/**
		 * @brief Draw indexed geometry
		 * @param vertexArray The vertex array containing the geometry to draw
		 * @details Renders the geometry defined by the vertex array using indexed drawing
		 */
		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}

	private:
		static RendererAPI* s_RendererAPI;  ///< Pointer to the current renderer API implementation
	};

}