#include "zgpch.h"
#include <Zgine.h>

#include "imgui.h"

class BatchRendererTestLayer : public Zgine::Layer
{
public:
	BatchRendererTestLayer()
		: Layer("BatchRendererTest")
	{
		// Initialize batch renderer
		Zgine::BatchRenderer2D::Init();

		// Create some test textures
		m_RedTexture = Zgine::Texture2D::Create(1, 1);
		uint32_t redTextureData = 0xff0000ff; // Red color
		m_RedTexture->SetData(&redTextureData, sizeof(uint32_t));

		m_GreenTexture = Zgine::Texture2D::Create(1, 1);
		uint32_t greenTextureData = 0x00ff00ff; // Green color
		m_GreenTexture->SetData(&greenTextureData, sizeof(uint32_t));

		m_BlueTexture = Zgine::Texture2D::Create(1, 1);
		uint32_t blueTextureData = 0x0000ffff; // Blue color
		m_BlueTexture->SetData(&blueTextureData, sizeof(uint32_t));
	}

	virtual ~BatchRendererTestLayer()
	{
		Zgine::BatchRenderer2D::Shutdown();
	}

	virtual void OnUpdate() override
	{
		// Update camera position based on input
		if (Zgine::Input::IsKeyPressed(ZG_KEY_A))
			m_CameraPosition.x -= m_CameraSpeed;
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_D))
			m_CameraPosition.x += m_CameraSpeed;

		if (Zgine::Input::IsKeyPressed(ZG_KEY_W))
			m_CameraPosition.y += m_CameraSpeed;
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_S))
			m_CameraPosition.y -= m_CameraSpeed;

		// Render with batch renderer
		Zgine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Zgine::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		Zgine::BatchRenderer2D::BeginScene(m_Camera);

		// Draw multiple quads with different colors and textures
		for (int i = 0; i < 100; i++)
		{
			float x = (i % 10) * 0.2f - 1.0f;
			float y = (i / 10) * 0.2f - 1.0f;
			
			glm::vec4 color = glm::vec4(
				(float)(i % 3) / 2.0f,
				(float)((i + 1) % 3) / 2.0f,
				(float)((i + 2) % 3) / 2.0f,
				1.0f
			);

			if (i % 3 == 0)
				Zgine::BatchRenderer2D::DrawQuad({ x, y, 0.0f }, { 0.15f, 0.15f }, m_RedTexture, color);
			else if (i % 3 == 1)
				Zgine::BatchRenderer2D::DrawQuad({ x, y, 0.0f }, { 0.15f, 0.15f }, m_GreenTexture, color);
			else
				Zgine::BatchRenderer2D::DrawQuad({ x, y, 0.0f }, { 0.15f, 0.15f }, m_BlueTexture, color);
		}

		// Draw some rotated quads
		for (int i = 0; i < 20; i++)
		{
			float angle = m_Time + i * 0.1f;
			float x = cos(angle) * 0.5f;
			float y = sin(angle) * 0.5f;
			
			Zgine::BatchRenderer2D::DrawRotatedQuad(
				{ x, y, 0.0f }, 
				{ 0.1f, 0.1f }, 
				angle, 
				glm::vec4(1.0f, 1.0f, 1.0f, 0.8f)
			);
		}

		Zgine::BatchRenderer2D::EndScene();

		m_Time += 0.016f; // Approximate 60 FPS
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Batch Renderer Test");
		ImGui::Text("Batch Renderer 2D Test");
		ImGui::Text("Use WASD to move camera");
		
		auto stats = Zgine::BatchRenderer2D::GetStats();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quad Count: %d", stats.QuadCount);
		ImGui::Text("Vertex Count: %d", stats.VertexCount);
		ImGui::Text("Index Count: %d", stats.IndexCount);
		
		ImGui::Text("Time: %.2f", m_Time);
		
		if (ImGui::Button("Reset Stats"))
		{
			Zgine::BatchRenderer2D::ResetStats();
		}
		
		ImGui::End();
	}

private:
	std::shared_ptr<Zgine::Texture2D> m_RedTexture;
	std::shared_ptr<Zgine::Texture2D> m_GreenTexture;
	std::shared_ptr<Zgine::Texture2D> m_BlueTexture;
	Zgine::OrthographicCamera m_Camera{ -1.6f, 1.6f, -0.9f, 0.9f };
	glm::vec3 m_CameraPosition{ 0.0f };
	float m_CameraSpeed = 0.01f;
	float m_Time = 0.0f;
};

class Sandbox : public Zgine::Application {
public:
	Sandbox()
	{
		PushLayer(new BatchRendererTestLayer());
	}
	~Sandbox() {};
};

Zgine::Application* Zgine::CreateApplication() {
	return new Sandbox();
};