#pragma once

#include "Zgine/Layer.h"
#include "Zgine/Renderer/OrthographicCamera.h"
#include "Zgine/Renderer/PerspectiveCamera.h"
#include "Zgine/Renderer/BatchRenderer2D.h"
#include "Zgine/Renderer/BatchRenderer3D.h"
#include "Zgine/Renderer/ParticleSystem.h"
#include "Zgine/Input.h"
#include "Zgine/KeyCodes.h"
#include "Zgine/MouseButtonCodes.h"
#include "Zgine/Events/Event.h"
#include "Zgine/Events/ApplicationEvent.h"
#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>
#include <string>

namespace Sandbox {

	class MainControlLayer : public Zgine::Layer
	{
	public:
		MainControlLayer();
		virtual ~MainControlLayer();

		virtual void OnAttach() override;
		virtual void OnUpdate(Zgine::Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Zgine::Event& e) override;

	private:
		void RenderMainMenu();
		void Render2DTestWindow();
		void Render3DTestWindow();
		void RenderParticleSystemWindow();
		void RenderPerformanceWindow();
		void RenderSettingsWindow();

		// Event handling
		bool OnWindowResize(Zgine::WindowResizeEvent& e);

		// Particle System
		void InitializeParticleSystems();
		void UpdateParticleSystems(Zgine::Timestep ts);
		void RenderParticleSystems();

		// 2D Test Layer
		void Update2DCamera(Zgine::Timestep ts);
		void Render2DBasicShapes();
		void Render2DAdvancedShapes();
		void Render2DAnimatedShapes();

		// 3D Test Layer
		void Update3DCamera(Zgine::Timestep ts);
		void Render3DBasicShapes();
		void Render3DAnimatedShapes();
		void Render3DEnvironment();

		// Utility functions
		int GetTotalObjectCount() const;

	private:
		// Cameras
		Zgine::OrthographicCamera m_2DCamera;
		Zgine::PerspectiveCamera m_3DCamera;
		
		// 2D Camera properties
		glm::vec3 m_2DCameraPosition;
		float m_2DCameraSpeed;
		
		// 3D Camera properties
		glm::vec3 m_3DCameraPosition;
		glm::vec3 m_3DCameraRotation;
		float m_3DCameraSpeed;
		float m_3DRotationSpeed;
		
		// Time
		float m_Time;

		// Window states
		bool m_Show2DTestWindow;
		bool m_Show3DTestWindow;
		bool m_ShowPerformanceWindow;
		bool m_ShowSettingsWindow;

		// 2D Render options
		bool m_2DShowQuads;
		bool m_2DShowLines;
		bool m_2DShowCircles;
		bool m_2DShowAdvanced;
		bool m_2DShowTriangles;
		bool m_2DShowEllipses;
		bool m_2DShowArcs;
		bool m_2DShowGradients;
		bool m_2DAnimateCircles;
		bool m_2DAnimateQuads;
		float m_2DAnimationSpeed;
		float m_2DLineThickness;
		float m_2DCircleRadius;
		int m_2DCircleSegments;

		// 3D Render options
		bool m_3DShowCubes;
		bool m_3DShowSpheres;
		bool m_3DShowCylinders;
		bool m_3DShowPlanes;
		bool m_3DShowEnvironment;
		bool m_3DAnimateObjects;
		bool m_3DWireframeMode;
		float m_3DLightIntensity;
		glm::vec3 m_3DLightPosition;
		glm::vec3 m_3DLightColor;

		// Particle System
		bool m_ShowParticleSystem;
		bool m_ParticleSystemEnabled;
		Zgine::Ref<Zgine::ParticleSystem> m_FireParticleSystem;
		Zgine::Ref<Zgine::ParticleSystem> m_SmokeParticleSystem;
		Zgine::Ref<Zgine::ParticleSystem> m_ExplosionParticleSystem;
		float m_ParticleSystemIntensity;

		// Performance
		float m_FPS;
		int m_FrameCount;
		float m_FPSTimer;
	};

}
