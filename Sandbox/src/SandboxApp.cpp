#include "sandbox_pch.h"
#include <Zgine.h>
#include "MainControlLayer.h"
#include "Testing/ResourceTestLayer.h"
#include "Zgine/Logging/Log.h"
#include "Zgine/Core/SmartPointers.h"

class SandboxApp : public Zgine::Application
{
public:
	SandboxApp()
	{
		ZG_CORE_INFO("SandboxApp constructor called");
		// Don't create layers in constructor - wait for Application to be fully initialized
	}

	~SandboxApp()
	{
		// Layers are automatically cleaned up by Application destructor
	}

	virtual void OnApplicationStart() override
	{
		ZG_CORE_INFO("SandboxApp::OnApplicationStart called - Creating MainControlLayer");
		// Push main control layer after Application and Renderer are fully initialized
		PushLayer(Zgine::CreateScope<Sandbox::MainControlLayer>().release());
		ZG_CORE_INFO("MainControlLayer created and pushed to layer stack");
		
		// Push Resource Test Layer for testing audio and resource management
		PushLayer(Zgine::CreateScope<Sandbox::ResourceTestLayer>().release());
		ZG_CORE_INFO("ResourceTestLayer created and pushed to layer stack");
	}
};

Zgine::Application* Zgine::CreateApplication()
{
	ZG_CORE_INFO("CreateApplication called - Creating SandboxApp");
	return Zgine::CreateScope<SandboxApp>().release();
}