#pragma once

#include "RenderCommand.h"
#include "VertexArray.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "Shader.h"

namespace Zgine {

	/**
	 * @brief High-level renderer class for managing rendering operations
	 * @details This class provides a high-level interface for rendering operations,
	 *          managing scene data, and coordinating between different rendering components
	 */
	class Renderer
	{
	public:
		/**
		 * @brief Initialize the renderer system
		 * @details Sets up the renderer and initializes all rendering components
		 */
		static void Init();
		
		/**
		 * @brief Shutdown the renderer system
		 * @details Cleans up all rendering resources and shuts down components
		 */
		static void Shutdown();

		/**
		 * @brief Begin rendering a scene with orthographic camera
		 * @param camera The orthographic camera to use for rendering
		 * @details Sets up the scene data for orthographic projection
		 */
		static void BeginScene(OrthographicCamera& camera);
		
		/**
		 * @brief Begin rendering a scene with perspective camera
		 * @param camera The perspective camera to use for rendering
		 * @details Sets up the scene data for perspective projection
		 */
		static void BeginScene(PerspectiveCamera& camera);
		
		/**
		 * @brief End the current scene rendering
		 * @details Finalizes the current scene and prepares for next frame
		 */
		static void EndScene();

		/**
		 * @brief Submit a draw call for rendering
		 * @param shader The shader to use for rendering
		 * @param vertexArray The vertex array containing geometry data
		 * @details Submits geometry to be rendered with the specified shader
		 */
		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray);

		/**
		 * @brief Get the current rendering API
		 * @return RendererAPI::API The current rendering API
		 * @details Returns the currently active rendering API (OpenGL, DirectX, etc.)
		 */
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		/**
		 * @brief Scene data structure
		 * @details Contains data needed for rendering the current scene
		 */
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;  ///< Combined view and projection matrix
		};

		static SceneData* m_SceneData;  ///< Pointer to current scene data
	};

} 