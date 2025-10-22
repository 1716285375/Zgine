#include "zgpch.h"
#include <Zgine.h>
#include "Test2DLayer.h"
#include "Test3DLayer.h"

class SandboxApp : public Zgine::Application
{
public:
	SandboxApp()
	{
		// Push test layers
		PushLayer(new Sandbox::Test2DLayer());
		PushLayer(new Sandbox::Test3DLayer());
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