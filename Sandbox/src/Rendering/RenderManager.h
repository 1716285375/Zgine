#pragma once

#include "Zgine/Renderer/OrthographicCamera.h"
#include "Zgine/Renderer/PerspectiveCamera.h"
#include "Zgine/Events/Event.h"
#include <glm/glm.hpp>

namespace Sandbox {

	// Forward declarations
	class Test2DModule;
	class Test3DModule;

	class RenderManager
	{
	public:
		RenderManager();
		~RenderManager();

		void OnAttach();
		void OnUpdate(float ts);
		void OnEvent(Zgine::Event& e);

		// Camera access
		Zgine::OrthographicCamera& Get2DCamera() { return m_2DCamera; }
		Zgine::PerspectiveCamera& Get3DCamera() { return m_3DCamera; }

		// Test module access
		Test2DModule& GetTest2DModule() { return *m_Test2DModule; }
		Test3DModule& GetTest3DModule() { return *m_Test3DModule; }

		// Rendering
		void Render2D();
		void Render3D();

		// Performance
		float Get2DFPS() const;
		float Get3DFPS() const;
		int Get2DObjectCount() const;
		int Get3DObjectCount() const;

	private:
		Zgine::OrthographicCamera m_2DCamera;
		Zgine::PerspectiveCamera m_3DCamera;
		float m_Time = 0.0f;

		// Test modules - using pointers to avoid incomplete type issues
		Test2DModule* m_Test2DModule = nullptr;
		Test3DModule* m_Test3DModule = nullptr;
	};

}