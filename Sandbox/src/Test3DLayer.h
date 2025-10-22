#pragma once

#include <Zgine.h>
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Sandbox {

	class Test3DLayer : public Zgine::Layer
	{
	public:
		Test3DLayer();
		virtual ~Test3DLayer();

		virtual void OnUpdate(Zgine::Timestep ts) override;
		virtual void OnImGuiRender() override;

	private:
		void UpdateCamera(Zgine::Timestep ts);
		void UpdateAnimations(Zgine::Timestep ts);
		void RenderBasicShapes();
		void RenderAnimatedShapes();
		void RenderEnvironment();

	private:
		Zgine::PerspectiveCamera m_Camera;
		glm::vec3 m_CameraPosition;
		glm::vec3 m_CameraRotation;
		float m_CameraSpeed;
		float m_RotationSpeed;
		float m_Time;

		// Render options
		bool m_ShowCubes;
		bool m_ShowSpheres;
		bool m_ShowPlanes;
		bool m_ShowEnvironment;
		bool m_AnimateObjects;
		bool m_WireframeMode;

		// Settings
		float m_LightIntensity;
		glm::vec3 m_LightPosition;
		glm::vec3 m_LightColor;

		// Performance
		float m_FPS;
		int m_FrameCount;
		float m_FPSTimer;
	};

}
