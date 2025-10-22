#include "zgpch.h"
#include <Zgine.h>
#include "MainControlLayer.h"

class SandboxApp : public Zgine::Application
{
public:
	SandboxApp()
	{
		// Don't create layers in constructor - wait for Application to be fully initialized
	}

	~SandboxApp()
	{
		// Layers are automatically cleaned up by Application destructor
	}

	virtual void OnApplicationStart() override
	{
		// Push main control layer after Application and Renderer are fully initialized
		PushLayer(new Sandbox::MainControlLayer());
	}
};

Zgine::Application* Zgine::CreateApplication()
{
	return new SandboxApp();
}