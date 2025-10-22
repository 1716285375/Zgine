#pragma once

#include "RenderCommand.h"
#include "VertexArray.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "Shader.h"

namespace Zgine {
	

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(OrthographicCamera& camera);
		static void BeginScene(PerspectiveCamera& camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;

	};
} 