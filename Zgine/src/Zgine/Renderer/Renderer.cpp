#include "zgpch.h"
#include "Renderer.h"
#include "BatchRenderer2D.h"
#include "BatchRenderer3D.h"
#include "RendererManager.h"


namespace Zgine {

	Renderer::SceneData* Renderer::m_SceneData = nullptr;

	void Renderer::Init()
	{
		// Initialize scene data
		if (!m_SceneData)
			m_SceneData = new Renderer::SceneData;
			
		// Use RendererManager for safe initialization
		RendererManager::GetInstance().Init();
	}

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

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		if (m_SceneData)
			m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::BeginScene(PerspectiveCamera& camera)
	{
		if (m_SceneData)
			m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{

	}

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