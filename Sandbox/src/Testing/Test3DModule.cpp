#include "Test3DModule.h"
#include "Zgine/Core.h"
#include "Zgine/Log.h"
#include "Zgine/Events/ApplicationEvent.h"
#include "Zgine/Events/KeyEvent.h"
#include "Zgine/Events/MouseEvent.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <GL/gl.h>

namespace Sandbox {

	Test3DModule::Test3DModule()
		: m_Camera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f)
		, m_Time(0.0f)
		, m_ActiveScene("Basic Shapes")
	{
		m_Camera.SetPosition(m_Config.cameraPosition);
		m_Camera.SetRotation(m_Config.cameraRotation);
		ZG_CORE_INFO("Test3DModule created");
	}

	Test3DModule::~Test3DModule()
	{
		ZG_CORE_INFO("Test3DModule destroyed");
	}

	void Test3DModule::OnAttach()
	{
		ZG_CORE_INFO("Test3DModule attached");
		
		// Register built-in scenes
		RegisterScene("Basic Shapes", [this](const Test3DConfig& config) {
			RenderBasicShapesScene(config);
		});
		
		RegisterScene("Animated Shapes", [this](const Test3DConfig& config) {
			RenderAnimatedShapesScene(config);
		});
		
		RegisterScene("Environment", [this](const Test3DConfig& config) {
			RenderEnvironmentScene(config);
		});
		
		RegisterScene("Performance Test", [this](const Test3DConfig& config) {
			RenderPerformanceTestScene(config);
		});
		
		RegisterScene("Lighting Test", [this](const Test3DConfig& config) {
			RenderLightingTestScene(config);
		});
	}

	void Test3DModule::OnUpdate(float ts)
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

		// Reset object count for this frame
		ResetObjectCount();
	}

	void Test3DModule::OnImGuiRender()
	{
		// UI is now handled by UIManager to avoid duplicate windows
		// This method is kept for compatibility but does nothing
	}

	void Test3DModule::OnEvent(Zgine::Event& e)
	{
		Zgine::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Zgine::WindowResizeEvent>([this](Zgine::WindowResizeEvent& e) {
			float aspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
			m_Camera.SetAspectRatio(aspectRatio);
			return false;
		});
	}

	void Test3DModule::RegisterScene(const std::string& name, std::function<void(const Test3DConfig&)> renderFunc)
	{
		m_Scenes.emplace_back(name, renderFunc);
		ZG_CORE_INFO("Registered 3D scene: {}", name);
	}

	void Test3DModule::SetActiveScene(const std::string& name)
	{
		m_ActiveScene = name;
		ZG_CORE_INFO("Active 3D scene set to: {}", name);
	}

	void Test3DModule::BeginScene()
	{
		Zgine::BatchRenderer3D::BeginScene(m_Camera);
	}

	void Test3DModule::EndScene()
	{
		Zgine::BatchRenderer3D::EndScene();
	}

	void Test3DModule::RenderActiveScene()
	{
		for (auto& scene : m_Scenes)
		{
			if (scene.GetName() == m_ActiveScene)
			{
				scene.Render(m_Config);
				break;
			}
		}
	}

	void Test3DModule::UpdateCamera(float ts)
	{
		// Update camera position based on input
		glm::vec3 front = m_Camera.GetForward();
		glm::vec3 right = m_Camera.GetRight();
		glm::vec3 up = m_Camera.GetUp();

		if (Zgine::Input::IsKeyPressed(ZG_KEY_W))
			m_Config.cameraPosition += front * m_Config.cameraSpeed * ts;
		if (Zgine::Input::IsKeyPressed(ZG_KEY_S))
			m_Config.cameraPosition -= front * m_Config.cameraSpeed * ts;
		if (Zgine::Input::IsKeyPressed(ZG_KEY_A))
			m_Config.cameraPosition -= right * m_Config.cameraSpeed * ts;
		if (Zgine::Input::IsKeyPressed(ZG_KEY_D))
			m_Config.cameraPosition += right * m_Config.cameraSpeed * ts;
		if (Zgine::Input::IsKeyPressed(ZG_KEY_SPACE))
			m_Config.cameraPosition += up * m_Config.cameraSpeed * ts;
		if (Zgine::Input::IsKeyPressed(ZG_KEY_LEFT_SHIFT))
			m_Config.cameraPosition -= up * m_Config.cameraSpeed * ts;

		// Update camera rotation based on mouse input
		if (Zgine::Input::IsMouseButtonPressed(ZG_MOUSE_BUTTON_RIGHT))
		{
		auto mousePos = Zgine::Input::GetMousePosition();
		static auto lastMousePos = mousePos;
		
		float deltaX = mousePos.first - lastMousePos.first;
		float deltaY = mousePos.second - lastMousePos.second;
			
			m_Config.cameraRotation.y += deltaX * m_Config.rotationSpeed * ts;
			m_Config.cameraRotation.x -= deltaY * m_Config.rotationSpeed * ts;
			
			// Clamp pitch
			m_Config.cameraRotation.x = glm::clamp(m_Config.cameraRotation.x, -89.0f, 89.0f);
			
			lastMousePos = mousePos;
		}
		else
		{
			auto mousePos = Zgine::Input::GetMousePosition();
			static auto lastMousePos = mousePos;
			lastMousePos = mousePos;
		}

		// Update camera
		m_Camera.SetPosition(m_Config.cameraPosition);
		m_Camera.SetRotation(m_Config.cameraRotation);
	}

	void Test3DModule::UpdateAnimations(float ts)
	{
		m_Time += ts;
	}

	void Test3DModule::RenderUI()
	{
		if (m_ShowConfigWindow)
			RenderConfigWindow();
		if (m_ShowPerformanceWindow)
			RenderPerformanceWindow();
		if (m_ShowSceneSelector)
			RenderSceneSelector();
	}

	void Test3DModule::RenderConfigWindow()
	{
		if (ImGui::Begin("3D Test Configuration", &m_ShowConfigWindow))
		{
			ImGui::Text("Render Options");
			ImGui::Separator();
			ImGui::Checkbox("Show Cubes", &m_Config.showCubes);
			ImGui::Checkbox("Show Spheres", &m_Config.showSpheres);
			ImGui::Checkbox("Show Planes", &m_Config.showPlanes);
			ImGui::Checkbox("Show Environment", &m_Config.showEnvironment);
			ImGui::Checkbox("Animate Objects", &m_Config.animateObjects);
			ImGui::Checkbox("Wireframe Mode", &m_Config.wireframeMode);

			ImGui::Separator();
			ImGui::Text("Lighting");
			ImGui::SliderFloat("Light Intensity", &m_Config.lightIntensity, 0.0f, 5.0f);
			ImGui::SliderFloat3("Light Position", &m_Config.lightPosition.x, -20.0f, 20.0f);
			ImGui::ColorEdit3("Light Color", &m_Config.lightColor.x);

			ImGui::Separator();
			ImGui::Text("Camera Settings");
			ImGui::SliderFloat("Camera Speed", &m_Config.cameraSpeed, 1.0f, 20.0f);
			ImGui::SliderFloat("Rotation Speed", &m_Config.rotationSpeed, 10.0f, 180.0f);
			ImGui::SliderFloat3("Camera Position", &m_Config.cameraPosition.x, -50.0f, 50.0f);
			ImGui::SliderFloat3("Camera Rotation", &m_Config.cameraRotation.x, -180.0f, 180.0f);
		}
		ImGui::End();
	}

	void Test3DModule::RenderPerformanceWindow()
	{
		if (ImGui::Begin("3D Performance", &m_ShowPerformanceWindow))
		{
			ImGui::Text("Performance Metrics");
			ImGui::Separator();
			ImGui::Text("FPS: %.1f", m_FPS);
			ImGui::Text("Objects Rendered: %d", m_ObjectCount);
			ImGui::Text("Active Scene: %s", m_ActiveScene.c_str());
		}
		ImGui::End();
	}

	void Test3DModule::RenderSceneSelector()
	{
		if (ImGui::Begin("3D Scene Selector", &m_ShowSceneSelector))
		{
			ImGui::Text("Select 3D Test Scene");
			ImGui::Separator();

			for (const auto& scene : m_Scenes)
			{
				bool isSelected = (scene.GetName() == m_ActiveScene);
				if (ImGui::Selectable(scene.GetName().c_str(), isSelected))
				{
					SetActiveScene(scene.GetName());
				}
			}
		}
		ImGui::End();
	}

	void Test3DModule::RenderBasicShapesScene(const Test3DConfig& config)
	{
		// Apply render mode settings
		if (config.wireframeMode)
		{
			// Enable wireframe rendering
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			// Ensure normal rendering
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Render cubes
		if (config.showCubes)
		{
			ZG_CORE_TRACE("3D Basic Shapes Scene: Rendering cubes (showCubes=true)");
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					float x = -2.0f + i * 2.0f;
					float z = -2.0f + j * 2.0f;
					
					Zgine::BatchRenderer3D::DrawCube({ x, 0.0f, z }, { 1.0f, 1.0f, 1.0f }, 
						{ 1.0f, 0.0f, 0.0f, 1.0f });
					IncrementObjectCount();
				}
			}
		}
		else
		{
			ZG_CORE_TRACE("3D Basic Shapes Scene: Skipping cubes (showCubes=false)");
		}

		// Render spheres
		if (config.showSpheres)
		{
			ZG_CORE_TRACE("3D Basic Shapes Scene: Rendering spheres (showSpheres=true)");
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					float x = -1.0f + i * 2.0f;
					float z = -1.0f + j * 2.0f;
					
					Zgine::BatchRenderer3D::DrawSphere({ x, 2.0f, z }, 0.5f, 
						{ 0.0f, 1.0f, 0.0f, 1.0f });
					IncrementObjectCount();
				}
			}
		}
		else
		{
			ZG_CORE_TRACE("3D Basic Shapes Scene: Skipping spheres (showSpheres=false)");
		}

		// Render planes
		if (config.showPlanes)
		{
			ZG_CORE_TRACE("3D Basic Shapes Scene: Rendering planes (showPlanes=true)");
			Zgine::BatchRenderer3D::DrawPlane({ 0.0f, -1.0f, 0.0f }, { 10.0f, 10.0f }, 
				{ 0.5f, 0.5f, 0.5f, 1.0f });
			IncrementObjectCount();
		}
		else
		{
			ZG_CORE_TRACE("3D Basic Shapes Scene: Skipping planes (showPlanes=false)");
		}
	}

	void Test3DModule::RenderAnimatedShapesScene(const Test3DConfig& config)
	{
		if (!config.animateObjects)
			return;

		// Animated cubes
		if (config.showCubes)
		{
			for (int i = 0; i < 5; i++)
			{
				float x = -4.0f + i * 2.0f;
				float y = sin(m_Time + i) * 2.0f;
				float rotation = m_Time * 45.0f + i * 30.0f;
				
				Zgine::BatchRenderer3D::DrawCube({ x, y, 0.0f }, { 1.0f, 1.0f, 1.0f }, 
					{ 1.0f, 0.5f, 0.0f, 1.0f });
				IncrementObjectCount();
			}
		}

		// Animated spheres
		if (config.showSpheres)
		{
			for (int i = 0; i < 3; i++)
			{
				float x = -2.0f + i * 2.0f;
				float y = cos(m_Time * 1.5f + i) * 1.5f + 3.0f;
				float scale = 0.3f + sin(m_Time * 2.0f + i) * 0.2f;
				
				Zgine::BatchRenderer3D::DrawSphere({ x, y, 0.0f }, scale, 
					{ 0.0f, 1.0f, 1.0f, 1.0f });
				IncrementObjectCount();
			}
		}
	}

	void Test3DModule::RenderEnvironmentScene(const Test3DConfig& config)
	{
		if (!config.showEnvironment)
			return;

		// Ground plane
		Zgine::BatchRenderer3D::DrawPlane({ 0.0f, -2.0f, 0.0f }, { 20.0f, 20.0f }, 
			{ 0.2f, 0.6f, 0.2f, 1.0f });
		IncrementObjectCount();

		// Sky box (simplified as planes)
		Zgine::BatchRenderer3D::DrawPlane({ 0.0f, 10.0f, -10.0f }, { 20.0f, 20.0f }, 
			{ 0.5f, 0.8f, 1.0f, 1.0f });
		IncrementObjectCount();

		// Trees (simplified as cylinders)
		for (int i = 0; i < 5; i++)
		{
			float x = -8.0f + i * 4.0f;
			float z = -8.0f + (i % 2) * 16.0f;
			
			// Trunk
			Zgine::BatchRenderer3D::DrawCylinder({ x, 0.0f, z }, 0.2f, 3.0f, 
				{ 0.4f, 0.2f, 0.1f, 1.0f });
			IncrementObjectCount();
			
			// Leaves
			Zgine::BatchRenderer3D::DrawSphere({ x, 2.0f, z }, 1.0f, 
				{ 0.1f, 0.8f, 0.1f, 1.0f });
			IncrementObjectCount();
		}
	}

	void Test3DModule::RenderPerformanceTestScene(const Test3DConfig& config)
	{
		// Render many objects for performance testing
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> disPos(-10.0f, 10.0f);
		std::uniform_real_distribution<float> disColor(0.0f, 1.0f);

		for (int i = 0; i < 200; i++)
		{
			float x = disPos(gen);
			float y = disPos(gen) * 0.5f;
			float z = disPos(gen);
			glm::vec4 color = { disColor(gen), disColor(gen), disColor(gen), 1.0f };
			
			Zgine::BatchRenderer3D::DrawCube({ x, y, z }, { 0.5f, 0.5f, 0.5f }, color);
			IncrementObjectCount();
		}
	}

	void Test3DModule::RenderLightingTestScene(const Test3DConfig& config)
	{
		// Render objects to test lighting
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				float x = -2.0f + i * 2.0f;
				float z = -2.0f + j * 2.0f;
				
				Zgine::BatchRenderer3D::DrawCube({ x, 0.0f, z }, { 1.0f, 1.0f, 1.0f }, 
					{ 0.8f, 0.8f, 0.8f, 1.0f });
				IncrementObjectCount();
			}
		}

		// Light source visualization
		Zgine::BatchRenderer3D::DrawSphere(m_Config.lightPosition, 0.2f, 
			{ m_Config.lightColor.x, m_Config.lightColor.y, m_Config.lightColor.z, 1.0f });
		IncrementObjectCount();
	}

}
