#include "sandbox_pch.h"
#include "RenderManager.h"
#include "Zgine/Logging/Log.h"
#include "Zgine/Events/ApplicationEvent.h"
#include "Zgine/Renderer/BatchRenderer2D.h"
#include "Zgine/Renderer/BatchRenderer3D.h"
#include "Zgine/Core/SmartPointers.h"
#include "../Testing/Test2DModule.h"
#include "../Testing/Test3DModule.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Sandbox {

	RenderManager::RenderManager()
		: m_2DCamera(-1.6f, 1.6f, -0.9f, 0.9f),
		  m_3DCamera(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f)
	{
		ZG_CORE_INFO("RenderManager created");
	}

	RenderManager::~RenderManager()
	{
		ZG_CORE_INFO("RenderManager destroyed");
	}

	void RenderManager::OnAttach()
	{
		ZG_CORE_INFO("RenderManager attached");
		
		// Initialize test modules
		m_Test2DModule = Zgine::CreateScope<Test2DModule>();
		m_Test3DModule = Zgine::CreateScope<Test3DModule>();
		m_Test2DModule->OnAttach();
		m_Test3DModule->OnAttach();
	}

	void RenderManager::OnUpdate(float ts)
	{
		m_Time += ts;
		
		// Update test modules
		if (m_Test2DModule) m_Test2DModule->OnUpdate(ts);
		if (m_Test3DModule) m_Test3DModule->OnUpdate(ts);
	}

	void RenderManager::OnEvent(Zgine::Event& e)
	{
		// Forward events to test modules
		if (m_Test2DModule) m_Test2DModule->OnEvent(e);
		if (m_Test3DModule) m_Test3DModule->OnEvent(e);

		// Handle window resize events
		Zgine::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Zgine::WindowResizeEvent>([this](Zgine::WindowResizeEvent& e) {
			float aspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
			
			// Update 2D camera
			m_2DCamera.SetProjection(-1.6f * aspectRatio, 1.6f * aspectRatio, -0.9f, 0.9f);
			
			// Update 3D camera
			m_3DCamera.SetAspectRatio(aspectRatio);
			
			return false;
		});
	}

	void RenderManager::Render2D()
	{
		if (m_Test2DModule)
		{
			Zgine::BatchRenderer2D::BeginScene(m_Test2DModule->GetCamera());
			m_Test2DModule->RenderActiveScene();
			Zgine::BatchRenderer2D::EndScene();
		}
	}

	void RenderManager::Render3D()
	{
		// ZG_CORE_TRACE("RenderManager::Render3D called");
		if (m_Test3DModule)
		{
			// ZG_CORE_TRACE("RenderManager::Render3D - Calling BatchRenderer3D::BeginScene");
			Zgine::BatchRenderer3D::BeginScene(m_Test3DModule->GetCamera());
			// ZG_CORE_TRACE("RenderManager::Render3D - Calling Test3DModule::RenderActiveScene");
			m_Test3DModule->RenderActiveScene();
			// ZG_CORE_TRACE("RenderManager::Render3D - Calling BatchRenderer3D::EndScene");
			Zgine::BatchRenderer3D::EndScene();
		}
		else
		{
			ZG_CORE_WARN("RenderManager::Render3D called but m_Test3DModule is null!");
		}
	}

	float RenderManager::Get2DFPS() const
	{
		return m_Test2DModule ? m_Test2DModule->GetFPS() : 0.0f;
	}

	float RenderManager::Get3DFPS() const
	{
		return m_Test3DModule ? m_Test3DModule->GetFPS() : 0.0f;
	}

	int RenderManager::Get2DObjectCount() const
	{
		return m_Test2DModule ? m_Test2DModule->GetObjectCount() : 0;
	}

	int RenderManager::Get3DObjectCount() const
	{
		return m_Test3DModule ? m_Test3DModule->GetObjectCount() : 0;
	}

}