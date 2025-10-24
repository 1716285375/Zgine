#include "Test3DLayer.h"
#include "Zgine/Log.h"

namespace Sandbox {

	Test3DLayer::Test3DLayer()
		: ImGuiLayer()
	{
		ZG_CORE_INFO("Test3DLayer created");
	}

	Test3DLayer::~Test3DLayer()
	{
		ZG_CORE_INFO("Test3DLayer destroyed");
	}

	void Test3DLayer::OnAttach()
	{
		ZG_CORE_INFO("Test3DLayer attached");
		m_Test3DModule.OnAttach();
	}

	void Test3DLayer::OnUpdate(Zgine::Timestep ts)
	{
		m_Test3DModule.OnUpdate(ts);
		
		// Begin 3D rendering
		m_Test3DModule.BeginScene();
		
		// Render 3D test scene
		m_Test3DModule.RenderActiveScene();
		
		// End 3D rendering
		m_Test3DModule.EndScene();
	}

	void Test3DLayer::OnImGuiRender()
	{
		m_Test3DModule.OnImGuiRender();
	}

	void Test3DLayer::OnEvent(Zgine::Event& e)
	{
		m_Test3DModule.OnEvent(e);
	}

}