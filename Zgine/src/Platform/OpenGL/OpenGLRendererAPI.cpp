#include "zgpch.h"
#include "OpenGLRendererAPI.h"
#include <glad/glad.h>

namespace Zgine {

	/**
	 * @brief Set the clear color for the renderer
	 * @param color The RGBA color to use for clearing the screen
	 * @details Sets the background color that will be used when clearing the screen
	 */
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	/**
	 * @brief Clear the screen
	 * @details Clears the color and depth buffers with the current clear color
	 */
	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	/**
	 * @brief Draw indexed geometry
	 * @param vertexArray The vertex array containing the geometry to draw
	 * @details Renders the geometry defined by the vertex array using indexed drawing
	 */
	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

}