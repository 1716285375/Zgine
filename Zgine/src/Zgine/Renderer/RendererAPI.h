#pragma once

#include <glm/glm.hpp>
#include "VertexArray.h"

namespace Zgine {

	/**
	 * @brief Abstract base class for rendering API implementations
	 * @details This class defines the interface that all rendering API implementations
	 *          must follow, providing platform-independent rendering operations
	 */
	class RendererAPI
	{
	public:
		/**
		 * @brief Enumeration of supported rendering APIs
		 * @details Defines all available rendering API implementations
		 */
		enum class API
		{
			None = 0,    ///< No API selected
			OpenGL = 1   ///< OpenGL rendering API
		};

	public:
		/**
		 * @brief Set the clear color for the renderer
		 * @param color The RGBA color to use for clearing the screen
		 * @details Sets the background color that will be used when clearing the screen
		 */
		virtual void SetClearColor(const glm::vec4& color) = 0;
		
		/**
		 * @brief Clear the screen
		 * @details Clears the color and depth buffers with the current clear color
		 */
		virtual void Clear() = 0;

		/**
		 * @brief Draw indexed geometry
		 * @param vertexArray The vertex array containing the geometry to draw
		 * @details Renders the geometry defined by the vertex array using indexed drawing
		 */
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;
		
		/**
		 * @brief Get the current rendering API
		 * @return API The currently active rendering API
		 * @details Returns the currently active rendering API implementation
		 */
		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;  ///< Currently active rendering API
	};

}