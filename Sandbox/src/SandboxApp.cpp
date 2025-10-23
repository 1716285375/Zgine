#include "zgpch.h"
#include <Zgine.h>
#include "MainControlLayer.h"
#include "Zgine/Log.h"

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
		PushLayer(new Sandbox::MainControlLayer());
		ZG_CORE_INFO("MainControlLayer created and pushed to layer stack");
	}
};

Zgine::Application* Zgine::CreateApplication()
{
	ZG_CORE_INFO("CreateApplication called - Creating SandboxApp");
	return new SandboxApp();
}