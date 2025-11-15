
#include <Zgine.h>

#include "imgui.h"

using namespace Zgine;

class ExampleLayer : public Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
		m_VertexArray.reset(Zgine::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		};

		std::shared_ptr<Zgine::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Zgine::VertexBuffer::Create(vertices, sizeof(vertices)));

		Zgine::BufferLayout layout = {
			{ Zgine::ShaderDataType::Float3, "a_Position"},
			{ Zgine::ShaderDataType::Float4, "a_Color" }
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };

		std::shared_ptr<Zgine::IndexBuffer> indexBuffer;
		indexBuffer.reset(Zgine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquaredVA.reset(Zgine::VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			0.75f, -0.75f, 0.0f,
			0.75f, 0.75f, 0.0f,
			-0.75f, 0.75f, 0.0f
		};

		std::shared_ptr<Zgine::VertexBuffer> squareVB(Zgine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Zgine::ShaderDataType::Float3, "a_Position"}
			});
		m_SquaredVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Zgine::IndexBuffer> squareIB(Zgine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquaredVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position; 
			layout(location = 1 ) in vec4 a_Color; 

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			
			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			
			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_BlueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));

	}
	virtual ~ExampleLayer() {

	}

	void OnUpdate(Zgine::Timestep ts) override
	{
		//ZG_TRACE("Delta time: {0}s {1}ms", ts.GetSeconds(), ts.GetMillisconds());

		if (Zgine::Input::IsKeyPressed(ZG_KEY_LEFT))
		{
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		}
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_RIGHT))
		{
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		}
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_UP))
		{
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		}
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_DOWN))
		{
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		}
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_A))
		{
			m_CameraRotation += m_CameraRotationSpeed * ts * 60;
		}
		else if (Zgine::Input::IsKeyPressed(ZG_KEY_D))
		{
			m_CameraRotation -= m_CameraRotationSpeed * ts * 60;
		}

		Zgine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Zgine::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Zgine::Renderer::BeginScene(m_Camera);
		Zgine::Renderer::Submit(m_BlueShader, m_SquaredVA);
		Zgine::Renderer::Submit(m_Shader, m_VertexArray);

		Zgine::Renderer::EndScene();
	}
	void OnEvent(Zgine::Event& event) override
	{
		Zgine::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Zgine::KeyPressedEvent>(ZG_BIND_EVENT_FN(ExampleLayer::OnKeyPressedEvent));
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Hello, ImGui!");
		ImGui::Text("This is some useful text.");
		ImGui::End();
	}


	bool OnKeyPressedEvent(Zgine::KeyPressedEvent& event)
	{
		return false;
	}

	private:
		std::shared_ptr<Zgine::Shader> m_Shader;
		std::shared_ptr<Zgine::VertexArray> m_VertexArray;

		std::shared_ptr<Zgine::Shader> m_BlueShader;
		std::shared_ptr<Zgine::VertexArray> m_SquaredVA;

		Zgine::OrthographicCamera m_Camera{ -1.6f, 1.6f, -0.9f, 0.9f };

		glm::vec3 m_CameraPosition{ 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraMoveSpeed = 1.0f;
		float m_CameraRotationSpeed = 0.5f;
};

class Sandbox : public Application {
	public:
		Sandbox()
		{
			PushLayer(new ExampleLayer());
		}
		~Sandbox() {};
};

namespace Zgine {
	Application* CreateApplication() {
		return new Sandbox();
	};
}


