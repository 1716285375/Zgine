#include "zgpch.h"
#include "Renderer.h"
#include "BatchRenderer2D.h"
#include "BatchRenderer3D.h"
#include "RendererManager.h"

namespace Zgine {

	Renderer::SceneData* Renderer::m_SceneData = nullptr;

	/**
	 * @brief Initialize the renderer system
	 * @details Sets up the renderer and initializes all rendering components
	 */
	void Renderer::Init()
	{
		// Initialize scene data
		if (!m_SceneData)
			m_SceneData = new Renderer::SceneData;
			
		// Use RendererManager for safe initialization
		RendererManager::GetInstance().Init();
	}

	/**
	 * @brief Shutdown the renderer system
	 * @details Cleans up all rendering resources and shuts down components
	 */
	void Renderer::Shutdown()
	{
		// Use RendererManager for safe shutdown
		RendererManager::GetInstance().Shutdown();
		
		// Clean up scene data
		if (m_SceneData)
		{
			delete m_SceneData;
			m_SceneData = nullptr;
		}
	}

	/**
	 * @brief Begin rendering a scene with orthographic camera
	 * @param camera The orthographic camera to use for rendering
	 * @details Sets up the scene data for orthographic projection
	 */
	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		if (m_SceneData)
			m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	/**
	 * @brief Begin rendering a scene with perspective camera
	 * @param camera The perspective camera to use for rendering
	 * @details Sets up the scene data for perspective projection
	 */
	void Renderer::BeginScene(PerspectiveCamera& camera)
	{
		if (m_SceneData)
			m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	/**
	 * @brief End the current scene rendering
	 * @details Finalizes the current scene and prepares for next frame
	 */
	void Renderer::EndScene()
	{
		// EndScene implementation
	}

	/**
	 * @brief Submit a draw call for rendering
	 * @param shader The shader to use for rendering
	 * @param vertexArray The vertex array containing geometry data
	 * @details Submits geometry to be rendered with the specified shader
	 */
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{
		if (!m_SceneData)
		{
			ZG_CORE_ERROR("Renderer::Submit called but scene data is not initialized!");
			return;
		}
		
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}