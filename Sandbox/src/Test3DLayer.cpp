#include "Test3DLayer.h"

namespace Sandbox {

	Test3DLayer::Test3DLayer()
		: Layer("Test3D")
		, m_Camera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f)
		, m_CameraPosition(0.0f, 5.0f, 10.0f)
		, m_CameraRotation(0.0f, 0.0f, 0.0f)
		, m_CameraSpeed(5.0f)
		, m_RotationSpeed(45.0f)
		, m_Time(0.0f)
		, m_ShowCubes(true)
		, m_ShowSpheres(true)
		, m_ShowPlanes(true)
		, m_ShowEnvironment(true)
		, m_AnimateObjects(true)
		, m_WireframeMode(false)
		, m_LightIntensity(1.0f)
		, m_LightPosition(0.0f, 10.0f, 0.0f)
		, m_LightColor(1.0f, 1.0f, 1.0f)
		, m_FPS(0.0f)
		, m_FrameCount(0)
		, m_FPSTimer(0.0f)
	{
		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
	}

	Test3DLayer::~Test3DLayer()
	{
		// 3D renderer shutdown is handled by Renderer::Shutdown()
	}

	void Test3DLayer::OnUpdate(Zgine::Timestep ts)
	{
		UpdateCamera(ts);
		UpdateAnimations(ts);

		// Calculate FPS
		m_FrameCount++;
		m_FPSTimer += ts;
		if (m_FPSTimer >= 1.0f)
		{
			m_FPS = m_FrameCount / m_FPSTimer;
			m_FrameCount = 0;
			m_FPSTimer = 0.0f;
		}

		// Render 3D scene
		Zgine::BatchRenderer3D::BeginScene(m_Camera);

		RenderEnvironment();
		RenderBasicShapes();
		if (m_AnimateObjects)
		{
			RenderAnimatedShapes();
		}

		Zgine::BatchRenderer3D::EndScene();
	}

	void Test3DLayer::UpdateCamera(Zgine::Timestep ts)
	{
		// Camera movement
		if (Zgine::Input::IsKeyPressed(ZG_KEY_W))
			m_Camera.MoveForward(m_CameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_S))
			m_Camera.MoveForward(-m_CameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_A))
			m_Camera.MoveRight(-m_CameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_D))
			m_Camera.MoveRight(m_CameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_Q))
			m_Camera.MoveUp(m_CameraSpeed * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_E))
			m_Camera.MoveUp(-m_CameraSpeed * ts);

		// Camera rotation with mouse
		if (Zgine::Input::IsMouseButtonPressed(ZG_MOUSE_BUTTON_RIGHT))
		{
			auto mousePos = Zgine::Input::GetMousePosition();
			static glm::vec2 lastMousePos = glm::vec2(mousePos.first, mousePos.second);
			
			if (lastMousePos != glm::vec2(-1.0f))
			{
				float deltaX = mousePos.first - lastMousePos.x;
				float deltaY = mousePos.second - lastMousePos.y;
				
				m_Camera.Rotate(deltaX * 0.1f, -deltaY * 0.1f);
			}
			
			lastMousePos = glm::vec2(mousePos.first, mousePos.second);
		}
	}

	void Test3DLayer::UpdateAnimations(Zgine::Timestep ts)
	{
		m_Time += ts;
	}

	void Test3DLayer::RenderEnvironment()
	{
		if (!m_ShowEnvironment)
			return;

		// Ground plane
		Zgine::BatchRenderer3D::DrawPlane(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec2(20.0f, 20.0f), 
			glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
		
		// Wall planes
		Zgine::BatchRenderer3D::DrawPlane(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec2(20.0f, 10.0f), 
			glm::vec4(0.2f, 0.4f, 0.2f, 1.0f));
		Zgine::BatchRenderer3D::DrawPlane(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec2(20.0f, 10.0f), 
			glm::vec4(0.4f, 0.2f, 0.2f, 1.0f));
		Zgine::BatchRenderer3D::DrawPlane(glm::vec3(-10.0f, 0.0f, 0.0f), glm::vec2(20.0f, 10.0f), 
			glm::vec4(0.2f, 0.2f, 0.4f, 1.0f));
		Zgine::BatchRenderer3D::DrawPlane(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec2(20.0f, 10.0f), 
			glm::vec4(0.4f, 0.4f, 0.2f, 1.0f));
	}

	void Test3DLayer::RenderBasicShapes()
	{
		if (m_ShowCubes)
		{
			// Static cubes
			Zgine::BatchRenderer3D::DrawCube({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 
				{1.0f, 0.0f, 0.0f, 1.0f});
			Zgine::BatchRenderer3D::DrawCube({2.0f, 0.0f, 0.0f}, {0.5f, 2.0f, 0.5f}, 
				{0.0f, 1.0f, 0.0f, 1.0f});
			Zgine::BatchRenderer3D::DrawCube({-2.0f, 0.0f, 0.0f}, {1.0f, 0.5f, 2.0f}, 
				{0.0f, 0.0f, 1.0f, 1.0f});
		}

		if (m_ShowSpheres)
		{
			// Static spheres
			Zgine::BatchRenderer3D::DrawSphere({0.0f, 2.0f, 0.0f}, 1.0f, 
				{1.0f, 1.0f, 0.0f, 1.0f}, 32);
			Zgine::BatchRenderer3D::DrawSphere({3.0f, 1.0f, 0.0f}, 0.8f, 
				{1.0f, 0.0f, 1.0f, 1.0f}, 32);
			Zgine::BatchRenderer3D::DrawSphere({-3.0f, 1.0f, 0.0f}, 0.6f, 
				{0.0f, 1.0f, 1.0f, 1.0f}, 32);
		}

		if (m_ShowPlanes)
		{
			// Additional planes
			Zgine::BatchRenderer3D::DrawPlane({0.0f, 5.0f, 0.0f}, {4.0f, 4.0f}, 
				{0.5f, 0.5f, 0.5f, 0.8f});
		}
	}

	void Test3DLayer::RenderAnimatedShapes()
	{
		if (m_ShowCubes)
		{
			// Animated cubes
			for (int i = 0; i < 5; i++)
			{
				float angle = m_Time * m_RotationSpeed + i * 72.0f;
				glm::vec3 position = glm::vec3(
					cos(glm::radians(angle)) * 3.0f,
					sin(m_Time * 2.0f + i) * 0.5f,
					sin(glm::radians(angle)) * 3.0f
				);
				
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * 
									 glm::rotate(glm::mat4(1.0f), m_Time * glm::radians(m_RotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
				
				Zgine::BatchRenderer3D::DrawCube(position, glm::vec3(0.5f), transform, 
					glm::vec4(0.2f + i * 0.15f, 0.3f + i * 0.1f, 0.8f - i * 0.1f, 1.0f));
			}
		}

		if (m_ShowSpheres)
		{
			// Animated spheres
			for (int i = 0; i < 3; i++)
			{
				float angle = m_Time * m_RotationSpeed * 0.5f + i * 120.0f;
				glm::vec3 position = glm::vec3(
					cos(glm::radians(angle)) * 5.0f,
					cos(m_Time * 1.5f + i) * 2.0f,
					sin(glm::radians(angle)) * 5.0f
				);
				
				Zgine::BatchRenderer3D::DrawSphere(position, 0.8f, 
					glm::vec4(0.8f - i * 0.2f, 0.2f + i * 0.3f, 0.4f + i * 0.2f, 1.0f), 32);
			}
		}
	}

	void Test3DLayer::OnImGuiRender()
	{
		// 3D Controls Window
		ImGui::Begin("3D Scene Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::Text("3D Rendering Controls");
		ImGui::Separator();
		
		ImGui::Checkbox("Show Cubes", &m_ShowCubes);
		ImGui::SameLine();
		ImGui::Checkbox("Show Spheres", &m_ShowSpheres);
		ImGui::SameLine();
		ImGui::Checkbox("Show Planes", &m_ShowPlanes);
		
		ImGui::Checkbox("Show Environment", &m_ShowEnvironment);
		ImGui::SameLine();
		ImGui::Checkbox("Animate Objects", &m_AnimateObjects);
		ImGui::SameLine();
		ImGui::Checkbox("Wireframe Mode", &m_WireframeMode);
		
		ImGui::Separator();
		ImGui::Text("Camera Settings");
		ImGui::SliderFloat("Camera Speed", &m_CameraSpeed, 1.0f, 20.0f);
		ImGui::SliderFloat("Rotation Speed", &m_RotationSpeed, 10.0f, 180.0f);
		
		ImGui::Separator();
		ImGui::Text("Lighting");
		ImGui::SliderFloat("Light Intensity", &m_LightIntensity, 0.1f, 3.0f);
		ImGui::SliderFloat3("Light Position", &m_LightPosition.x, -10.0f, 10.0f);
		ImGui::ColorEdit3("Light Color", &m_LightColor.x);
		
		ImGui::Separator();
		ImGui::Text("Controls:");
		ImGui::Text("WASD - Move camera");
		ImGui::Text("QE - Move up/down");
		ImGui::Text("Right Mouse + Drag - Rotate camera");
		
		ImGui::End();

		// 3D Performance Stats
		ImGui::Begin("3D Performance Stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		auto stats = Zgine::BatchRenderer3D::GetStats();
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Triangles: %d", stats.TriangleCount);
		ImGui::Text("Vertices: %d", stats.VertexCount);
		ImGui::Text("Indices: %d", stats.IndexCount);
		
		// FPS display with color coding
		ImVec4 fpsColor = {0.0f, 1.0f, 0.0f, 1.0f}; // Green
		if (m_FPS < 30.0f) fpsColor = {1.0f, 0.0f, 0.0f, 1.0f}; // Red
		else if (m_FPS < 60.0f) fpsColor = {1.0f, 1.0f, 0.0f, 1.0f}; // Yellow
		
		ImGui::TextColored(fpsColor, "FPS: %.1f", m_FPS);
		
		if (ImGui::Button("Reset Stats", ImVec2(120, 30)))
		{
			Zgine::BatchRenderer3D::ResetStats();
		}
		
		ImGui::End();

		// 3D Preset Configurations
		ImGui::Begin("3D Preset Configurations", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		
		ImGui::Text("Quick Presets:");
		ImGui::Separator();
		
		if (ImGui::Button("Basic Shapes", ImVec2(120, 30)))
		{
			m_ShowCubes = true;
			m_ShowSpheres = false;
			m_ShowPlanes = false;
			m_ShowEnvironment = true;
			m_AnimateObjects = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Spheres Only", ImVec2(120, 30)))
		{
			m_ShowCubes = false;
			m_ShowSpheres = true;
			m_ShowPlanes = false;
			m_ShowEnvironment = true;
			m_AnimateObjects = true;
		}
		
		if (ImGui::Button("Animated Scene", ImVec2(120, 30)))
		{
			m_ShowCubes = true;
			m_ShowSpheres = true;
			m_ShowPlanes = false;
			m_ShowEnvironment = true;
			m_AnimateObjects = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Minimal", ImVec2(120, 30)))
		{
			m_ShowCubes = true;
			m_ShowSpheres = false;
			m_ShowPlanes = false;
			m_ShowEnvironment = false;
			m_AnimateObjects = false;
		}
		
		if (ImGui::Button("Full Scene", ImVec2(120, 30)))
		{
			m_ShowCubes = true;
			m_ShowSpheres = true;
			m_ShowPlanes = true;
			m_ShowEnvironment = true;
			m_AnimateObjects = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Performance", ImVec2(120, 30)))
		{
			m_ShowCubes = true;
			m_ShowSpheres = false;
			m_ShowPlanes = false;
			m_ShowEnvironment = false;
			m_AnimateObjects = false;
		}
		
		ImGui::End();
	}

}
