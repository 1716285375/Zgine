#include "sandbox_pch.h"
#include "Test3DModule.h"
#include "Zgine/Core.h"
#include "Zgine/Logging/Log.h"
#include "Zgine/Events/ApplicationEvent.h"
#include "Zgine/Events/KeyEvent.h"
#include "Zgine/Events/MouseEvent.h"
#include "Zgine/ImGui/ImGuiWrapper.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <GL/gl.h>

namespace Sandbox {

	Test3DModule::Test3DModule()
		: m_Camera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f)
		, m_Time(0.0f)
		, m_ActiveScene("Basic Shapes")
	{
		// Explicitly set default configuration values
		m_Config.showCubes = true;
		m_Config.showSpheres = true;
		m_Config.showPlanes = true;
		m_Config.showEnvironment = false; // Changed from true to false
		m_Config.animateObjects = false; // Changed from true to false
		m_Config.wireframeMode = false;
		
		m_Camera.SetPosition(m_Config.cameraPosition);
		m_Camera.SetRotation(m_Config.cameraRotation);
		
		// Debug: Log camera setup
		ZG_CORE_INFO("Test3DModule - Camera Position: ({}, {}, {})", 
			m_Config.cameraPosition.x, m_Config.cameraPosition.y, m_Config.cameraPosition.z);
		ZG_CORE_INFO("Test3DModule - Camera Rotation: ({}, {}, {})", 
			m_Config.cameraRotation.x, m_Config.cameraRotation.y, m_Config.cameraRotation.z);
		ZG_CORE_INFO("Test3DModule - Camera Forward: ({}, {}, {})", 
			m_Camera.GetForward().x, m_Camera.GetForward().y, m_Camera.GetForward().z);
		
		// Debug: Log initial configuration values
		ZG_CORE_INFO("Test3DModule created - Initial config: showCubes={}, showSpheres={}, showPlanes={}", 
			m_Config.showCubes, m_Config.showSpheres, m_Config.showPlanes);
		
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
		// Handle camera input
		if (m_CameraControlEnabled)
		{
			HandleCameraInput(ts);
		}

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
		// Debug: Log current configuration and active scene
		// ZG_CORE_INFO("RenderActiveScene called - ActiveScene: {}, showCubes={}, showSpheres={}, showPlanes={}", 
		// 	m_ActiveScene, m_Config.showCubes, m_Config.showSpheres, m_Config.showPlanes);
		
		// Debug: Check if configuration was modified
		if (!m_Config.showCubes && !m_Config.showSpheres && !m_Config.showPlanes)
		{
			ZG_CORE_WARN("All 3D shapes are disabled! This might be a configuration issue.");
		}
		
		for (auto& scene : m_Scenes)
		{
			if (scene.GetName() == m_ActiveScene)
			{
				// ZG_CORE_INFO("Found matching scene: {}, calling render function", scene.GetName());
				scene.Render(m_Config);
				break;
			}
		}
	}

	void Test3DModule::UpdateCamera(float ts)
	{
		// Update camera with current configuration
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
		if (Zgine::IG::Begin("3D Test Configuration", &m_ShowConfigWindow))
		{
			Zgine::IG::Text("Render Options");
			Zgine::IG::Separator();
			Zgine::IG::Checkbox("Show Cubes", &m_Config.showCubes);
			Zgine::IG::Checkbox("Show Spheres", &m_Config.showSpheres);
			Zgine::IG::Checkbox("Show Planes", &m_Config.showPlanes);
			Zgine::IG::Checkbox("Show Environment", &m_Config.showEnvironment);
			Zgine::IG::Checkbox("Animate Objects", &m_Config.animateObjects);
			Zgine::IG::Checkbox("Wireframe Mode", &m_Config.wireframeMode);

			Zgine::IG::Separator();
			Zgine::IG::Text("Lighting");
			Zgine::IG::SliderFloat("Light Intensity", &m_Config.lightIntensity, 0.0f, 5.0f);
			Zgine::IG::SliderFloat3("Light Position", &m_Config.lightPosition.x, -20.0f, 20.0f);
			Zgine::IG::ColorEdit3("Light Color", &m_Config.lightColor.x);

			Zgine::IG::Separator();
			Zgine::IG::Text("Camera Settings");
			Zgine::IG::SliderFloat("Camera Speed", &m_Config.cameraSpeed, 1.0f, 20.0f);
			Zgine::IG::SliderFloat("Rotation Speed", &m_Config.rotationSpeed, 10.0f, 180.0f);
			Zgine::IG::SliderFloat3("Camera Position", &m_Config.cameraPosition.x, -50.0f, 50.0f);
			Zgine::IG::SliderFloat3("Camera Rotation", &m_Config.cameraRotation.x, -180.0f, 180.0f);
		}
		Zgine::IG::End();
	}

	void Test3DModule::RenderPerformanceWindow()
	{
		if (Zgine::IG::Begin("3D Performance", &m_ShowPerformanceWindow))
		{
			Zgine::IG::Text("Performance Metrics");
			Zgine::IG::Separator();
			Zgine::IG::Text("FPS: %.1f", m_FPS);
			Zgine::IG::Text("Objects Rendered: %d", m_ObjectCount);
			Zgine::IG::Text("Active Scene: %s", m_ActiveScene.c_str());
		}
		Zgine::IG::End();
	}

	void Test3DModule::RenderSceneSelector()
	{
		if (Zgine::IG::Begin("3D Scene Selector", &m_ShowSceneSelector))
		{
			Zgine::IG::Text("Select 3D Test Scene");
			Zgine::IG::Separator();

			for (const auto& scene : m_Scenes)
			{
				bool isSelected = (scene.GetName() == m_ActiveScene);
				if (Zgine::IG::Selectable(scene.GetName().c_str(), isSelected))
				{
					SetActiveScene(scene.GetName());
				}
			}
		}
		Zgine::IG::End();
	}

	void Test3DModule::RenderBasicShapesScene(const Test3DConfig& config)
	{
		// Debug: Log camera information
		// ZG_CORE_INFO("3D Basic Shapes Scene - Camera Position: ({}, {}, {})", 
		//	m_Camera.GetPosition().x, m_Camera.GetPosition().y, m_Camera.GetPosition().z);
		// ZG_CORE_INFO("3D Basic Shapes Scene - Camera Rotation: ({}, {}, {})", 
		//	m_Camera.GetRotation().x, m_Camera.GetRotation().y, m_Camera.GetRotation().z);
		// ZG_CORE_INFO("3D Basic Shapes Scene - Camera Forward: ({}, {}, {})", 
		//	m_Camera.GetForward().x, m_Camera.GetForward().y, m_Camera.GetForward().z);

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
			// ZG_CORE_TRACE("3D Basic Shapes Scene: Rendering cubes (showCubes=true)");
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					float x = -2.0f + i * 2.0f;
					float z = -2.0f + j * 2.0f;
					
					// ZG_CORE_INFO("3D Basic Shapes Scene - Drawing cube at ({}, {}, {})", x, 0.0f, z);
					Zgine::BatchRenderer3D::DrawCube({ x, 0.0f, z }, { 0.5f, 0.5f, 0.5f }, 
						{ 1.0f, 0.0f, 0.0f, 1.0f });
					IncrementObjectCount();
				}
			}
		}
		else
		{
			// ZG_CORE_TRACE("3D Basic Shapes Scene: Skipping cubes (showCubes=false)");
		}

		// Render spheres
		if (config.showSpheres)
		{
			// ZG_CORE_TRACE("3D Basic Shapes Scene: Rendering spheres (showSpheres=true)");
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					float x = -2.0f + i * 2.0f;
					float z = -2.0f + j * 2.0f;
					
					Zgine::BatchRenderer3D::DrawSphere({ x, 2.0f, z }, 0.5f, 
						{ 0.0f, 1.0f, 0.0f, 1.0f });
					IncrementObjectCount();
				}
			}
		}
		else
		{
			// ZG_CORE_TRACE("3D Basic Shapes Scene: Skipping spheres (showSpheres=false)");
		}

		// Render planes
		if (config.showPlanes)
		{
			// ZG_CORE_TRACE("3D Basic Shapes Scene: Rendering planes (showPlanes=true)");
			Zgine::BatchRenderer3D::DrawPlane({ 0.0f, -1.0f, 0.0f }, { 8.0f, 8.0f }, 
				{ 0.5f, 0.5f, 0.5f, 1.0f });
			IncrementObjectCount();
		}
		else
		{
			// ZG_CORE_TRACE("3D Basic Shapes Scene: Skipping planes (showPlanes=false)");
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

	void Test3DModule::HandleCameraInput(float ts)
	{
		HandleKeyboardInput(ts);
		HandleMouseInput(ts);
	}

	void Test3DModule::HandleKeyboardInput(float ts)
	{
		if (!m_Config.enableKeyboardMovement)
			return;

		glm::vec3 front = m_Camera.GetForward();
		glm::vec3 right = m_Camera.GetRight();
		glm::vec3 up = m_Camera.GetUp();

		// Movement controls
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

		// Speed controls
		if (Zgine::Input::IsKeyPressed(ZG_KEY_Q))
			m_Config.cameraSpeed = glm::max(0.1f, m_Config.cameraSpeed - 2.0f * ts);
		if (Zgine::Input::IsKeyPressed(ZG_KEY_E))
			m_Config.cameraSpeed = glm::min(50.0f, m_Config.cameraSpeed + 2.0f * ts);

		// Reset camera
		if (Zgine::Input::IsKeyPressed(ZG_KEY_R))
			ResetCamera();
	}

	void Test3DModule::HandleMouseInput(float ts)
	{
		if (!m_Config.enableMouseLook)
			return;

		// Check if right mouse button is pressed for mouse look
		if (Zgine::Input::IsMouseButtonPressed(ZG_MOUSE_BUTTON_RIGHT))
		{
			auto mousePos = Zgine::Input::GetMousePosition();
			
			if (m_FirstMouse)
			{
				m_LastMouseX = mousePos.first;
				m_LastMouseY = mousePos.second;
				m_FirstMouse = false;
				m_MouseCaptured = true;
			}

			float deltaX = mousePos.first - m_LastMouseX;
			float deltaY = mousePos.second - m_LastMouseY;

			// Apply mouse sensitivity
			deltaX *= m_Config.mouseSensitivity;
			deltaY *= m_Config.mouseSensitivity;

			// Update rotation
			m_Config.cameraRotation.y += deltaX;
			m_Config.cameraRotation.x -= deltaY;

			// Clamp pitch to prevent over-rotation
			m_Config.cameraRotation.x = glm::clamp(m_Config.cameraRotation.x, -89.0f, 89.0f);

			// Keep yaw in reasonable range
			if (m_Config.cameraRotation.y > 360.0f)
				m_Config.cameraRotation.y -= 360.0f;
			if (m_Config.cameraRotation.y < -360.0f)
				m_Config.cameraRotation.y += 360.0f;

			m_LastMouseX = mousePos.first;
			m_LastMouseY = mousePos.second;
		}
		else
		{
			m_FirstMouse = true;
			m_MouseCaptured = false;
		}
	}

	void Test3DModule::ResetCamera()
	{
		m_Config.cameraPosition = { 0.0f, 2.0f, 8.0f };
		m_Config.cameraRotation = { -15.0f, 0.0f, 0.0f };
		m_FirstMouse = true;
		m_MouseCaptured = false;
		ZG_CORE_INFO("3D Camera reset to default position");
	}

	void Test3DModule::SetCameraLookAt(const glm::vec3& target)
	{
		glm::vec3 direction = glm::normalize(target - m_Config.cameraPosition);
		
		// Calculate yaw and pitch from direction vector
		float yaw = glm::degrees(atan2(direction.z, direction.x));
		float pitch = glm::degrees(asin(-direction.y));
		
		m_Config.cameraRotation = { pitch, yaw, 0.0f };
		ZG_CORE_INFO("3D Camera looking at target: ({}, {}, {})", target.x, target.y, target.z);
	}

}
