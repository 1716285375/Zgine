#pragma once

#include <Zgine.h>
#include "imgui.h"

namespace Sandbox {

	class Test2DLayer : public Zgine::Layer
	{
	public:
		Test2DLayer();
		virtual ~Test2DLayer();

		virtual void OnUpdate(Zgine::Timestep ts) override;
		virtual void OnImGuiRender() override;

	private:
		void UpdateCamera(Zgine::Timestep ts);
		void UpdateAnimations(Zgine::Timestep ts);
		void RenderBasicShapes();
		void RenderAdvancedShapes();
		void RenderAnimatedShapes();

	private:
		Zgine::OrthographicCamera m_Camera;
		glm::vec3 m_CameraPosition;
		float m_CameraSpeed;
		float m_Time;

		// Render options
		bool m_ShowQuads;
		bool m_ShowLines;
		bool m_ShowCircles;
		bool m_ShowAdvanced;
		bool m_ShowTriangles;
		bool m_ShowEllipses;
		bool m_ShowArcs;
		bool m_ShowGradients;

		// Animation options
		bool m_AnimateCircles;
		bool m_AnimateQuads;
		float m_AnimationSpeed;

		// Settings
		float m_LineThickness;
		float m_CircleRadius;
		int m_CircleSegments;

		// Performance
		float m_FPS;
		int m_FrameCount;
		float m_FPSTimer;
	};

}
