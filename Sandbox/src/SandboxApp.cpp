#include "zgpch.h"
#include <Zgine.h>
#include "MainControlLayer.h"

class SandboxApp : public Zgine::Application
{
public:
	SandboxApp()
	{
		// Push main control layer
		PushLayer(new Sandbox::MainControlLayer());
	}

	~SandboxApp()
	{
		// Layers are automatically cleaned up by Application destructor
	}
};

Zgine::Application* Zgine::CreateApplication()
{
	return new SandboxApp();
}