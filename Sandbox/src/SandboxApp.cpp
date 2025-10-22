#include "zgpch.h"
#include <Zgine.h>

#include "imgui.h"

class MaterialTestLayer : public Zgine::Layer
{
public:
	MaterialTestLayer()
		: Layer("MaterialTest")
	{
		// Create a simple colored texture
		m_Texture = Zgine::Texture2D::Create(1, 1);
		uint32_t textureData = 0xffff00ff; // Yellow color
		m_Texture->SetData(&textureData, sizeof(uint32_t));

		// Create vertex array for textured quad
		m_VertexArray.reset(Zgine::VertexArray::Create());

		float vertices[4 * 5] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		std::shared_ptr<Zgine::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Zgine::VertexBuffer::Create(vertices, sizeof(vertices)));

		Zgine::BufferLayout layout = {
			{ Zgine::ShaderDataType::Float3, "a_Position" },
			{ Zgine::ShaderDataType::Float2, "a_TexCoord" }
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Zgine::IndexBuffer> indexBuffer;
		indexBuffer.reset(Zgine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		// Create texture shader
		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjection;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			
			in vec2 v_TexCoord;

			uniform sampler2D u_Texture;
			uniform vec4 u_Color;
			uniform float u_Time;

			void main()
			{
				vec4 texColor = texture(u_Texture, v_TexCoord);
				color = texColor * u_Color;
				
				// Add some animation based on time
				color.r += sin(u_Time) * 0.1;
				color.g += cos(u_Time) * 0.1;
			}
		)";

		m_Shader.reset(new Zgine::Shader(vertexSrc, fragmentSrc));

		// Create material
		m_Material = std::make_shared<Zgine::Material>("TestMaterial", m_Shader);
		m_Material->SetTexture("u_Texture", m_Texture);
		m_Material->SetFloat4("u_Color", glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));
		m_Material->SetFloat("u_Time", 0.0f);
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

		// Update time uniform
		m_Time += 0.016f; // Approximate 60 FPS
		m_Material->SetFloat("u_Time", m_Time);

		// Render textured quad with material
		Zgine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Zgine::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		Zgine::Renderer::BeginScene(m_Camera);

		m_Material->Bind();
		Zgine::Renderer::Submit(m_Shader, m_VertexArray);

		Zgine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Material Test");
		ImGui::Text("Material System Test");
		ImGui::Text("Use WASD to move camera");
		ImGui::Text("Material Name: %s", m_Material->GetName().c_str());
		ImGui::Text("Shader ID: %d", m_Shader->GetRendererID());
		ImGui::Text("Texture ID: %d", m_Texture->GetRendererID());
		ImGui::Text("Time: %.2f", m_Time);
		
		// Material property controls
		static glm::vec4 color = glm::vec4(1.0f, 0.5f, 0.2f, 1.0f);
		if (ImGui::ColorEdit4("Material Color", &color.x))
		{
			m_Material->SetFloat4("u_Color", color);
		}
		
		ImGui::End();
	}

private:
	std::shared_ptr<Zgine::Texture2D> m_Texture;
	std::shared_ptr<Zgine::VertexArray> m_VertexArray;
	std::shared_ptr<Zgine::Shader> m_Shader;
	std::shared_ptr<Zgine::Material> m_Material;
	Zgine::OrthographicCamera m_Camera{ -1.6f, 1.6f, -0.9f, 0.9f };
	glm::vec3 m_CameraPosition{ 0.0f };
	float m_CameraSpeed = 0.01f;
	float m_Time = 0.0f;
};

class Sandbox : public Zgine::Application {
public:
	Sandbox()
	{
		PushLayer(new MaterialTestLayer());
	}
	~Sandbox() {};
};

Zgine::Application* Zgine::CreateApplication() {
	return new Sandbox();
};