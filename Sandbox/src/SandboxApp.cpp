#include "zgpch.h"
#include <Zgine.h>
#include "Test2DLayer.h"
#include "Test3DLayer.h"

class Sandbox : public Zgine::Application
{
public:
	Sandbox()
	{
		// Push test layers
		PushLayer(new Test2DLayer());
		PushLayer(new Test3DLayer());
	}

	~Sandbox()
	{
		// Layers are automatically cleaned up by Application destructor
	}
};

Zgine::Application* Zgine::CreateApplication()
{
	return new Sandbox();
}