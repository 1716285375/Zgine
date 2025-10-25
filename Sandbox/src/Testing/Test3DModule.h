#pragma once

#include "sandbox_pch.h"
#include "Zgine/Core.h"
#include "Zgine/Renderer/PerspectiveCamera.h"
#include "Zgine/Renderer/BatchRenderer3D.h"
#include "Zgine/Input.h"
#include "Zgine/KeyCodes.h"
#include "Zgine/MouseButtonCodes.h"
#include "Zgine/Events/Event.h"


namespace Sandbox {

	// 3D Test Configuration
	struct Test3DConfig
	{
		// Render options
		bool showCubes = true;
		bool showSpheres = true;
		bool showPlanes = true;
		bool showEnvironment = false; // Changed from true to false
		bool animateObjects = false; // Changed from true to false
		bool wireframeMode = false;

		// Settings
		float lightIntensity = 1.0f;
		glm::vec3 lightPosition = { 0.0f, 10.0f, 0.0f };
		glm::vec3 lightColor = { 1.0f, 1.0f, 1.0f };

		// Camera settings
		float cameraSpeed = 5.0f;
		float rotationSpeed = 45.0f;
		float mouseSensitivity = 0.1f;
		bool enableMouseLook = true;
		bool enableKeyboardMovement = true;
		glm::vec3 cameraPosition = { 0.0f, 2.0f, 8.0f };
		glm::vec3 cameraRotation = { -15.0f, 0.0f, 0.0f };
	};

	// 3D Test Scene
	class Test3DScene
	{
	public:
		Test3DScene(const std::string& name, std::function<void(const Test3DConfig&)> renderFunc)
			: m_Name(name), m_RenderFunction(renderFunc) {}

		const std::string& GetName() const { return m_Name; }
		void Render(const Test3DConfig& config) { m_RenderFunction(config); }

	private:
		std::string m_Name;
		std::function<void(const Test3DConfig&)> m_RenderFunction;
	};

	// 3D Test Module Manager
	class Test3DModule
	{
	public:
		Test3DModule();
		~Test3DModule();

		void OnAttach();
		void OnUpdate(float ts);
		void OnImGuiRender();
		void OnEvent(Zgine::Event& e);

		// Configuration
		Test3DConfig& GetConfig() { return m_Config; }
		const Test3DConfig& GetConfig() const { return m_Config; }

		// Scene management
		void RegisterScene(const std::string& name, std::function<void(const Test3DConfig&)> renderFunc);
		void SetActiveScene(const std::string& name);
		const std::string& GetActiveScene() const { return m_ActiveScene; }
		const std::vector<Test3DScene>& GetScenes() const { return m_Scenes; }

		// Camera access
		Zgine::PerspectiveCamera& GetCamera() { return m_Camera; }
		const Zgine::PerspectiveCamera& GetCamera() const { return m_Camera; }

		// Camera control
		void EnableCameraControl(bool enable) { m_CameraControlEnabled = enable; }
		bool IsCameraControlEnabled() const { return m_CameraControlEnabled; }
		void ResetCamera();
		void SetCameraLookAt(const glm::vec3& target);

		// Performance
		float GetFPS() const { return m_FPS; }
		int GetObjectCount() const { return m_ObjectCount; }
		void ResetObjectCount() { m_ObjectCount = 0; }
		void IncrementObjectCount(int count = 1) { m_ObjectCount += count; }

		// Rendering
		void BeginScene();
		void EndScene();
		void RenderActiveScene();

		// UI state
		void SetShowConfigWindow(bool show) { m_ShowConfigWindow = show; }
		bool IsConfigWindowVisible() const { return m_ShowConfigWindow; }
		void SetShowPerformanceWindow(bool show) { m_ShowPerformanceWindow = show; }
		bool IsPerformanceWindowVisible() const { return m_ShowPerformanceWindow; }
		void SetShowSceneSelector(bool show) { m_ShowSceneSelector = show; }
		bool IsSceneSelectorVisible() const { return m_ShowSceneSelector; }

	private:
		void UpdateCamera(float ts);
		void UpdateAnimations(float ts);
		void RenderUI();
		void HandleCameraInput(float ts);
		void HandleMouseInput(float ts);
		void HandleKeyboardInput(float ts);

		// Built-in scenes
		void RenderBasicShapesScene(const Test3DConfig& config);
		void RenderAnimatedShapesScene(const Test3DConfig& config);
		void RenderEnvironmentScene(const Test3DConfig& config);
		void RenderPerformanceTestScene(const Test3DConfig& config);
		void RenderLightingTestScene(const Test3DConfig& config);

		// UI rendering
		void RenderConfigWindow();
		void RenderPerformanceWindow();
		void RenderSceneSelector();

	private:
		// Core components
		Zgine::PerspectiveCamera m_Camera;
		Test3DConfig m_Config;
		float m_Time;

		// Scene management
		std::vector<Test3DScene> m_Scenes;
		std::string m_ActiveScene;

		// Performance tracking
		float m_FPS = 0.0f;
		int m_FrameCount = 0;
		float m_FPSTimer = 0.0f;
		int m_ObjectCount = 0;

		// UI state
		bool m_ShowConfigWindow = true;
		bool m_ShowPerformanceWindow = false;
		bool m_ShowSceneSelector = true;

		// Camera control state
		bool m_CameraControlEnabled = true;
		bool m_FirstMouse = true;
		float m_LastMouseX = 0.0f;
		float m_LastMouseY = 0.0f;
		bool m_MouseCaptured = false;
	};

}
