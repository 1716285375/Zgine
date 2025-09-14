#include "zgpch.h"
#include <Zgine.h>

#include "imgui.h"
#include "Zgine/ImGui/ImGuiLayer.h"

class ExampleLayer : public Zgine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{

	}
	virtual ~ExampleLayer() {}
	void OnUpdate() override
	{
		//ZG_INFO("ExampleLayer Update");

		if (Zgine::Input::IsKeyPressed(ZG_KEY_TAB))
		{
			//ZG_INFO("Tab key is pressed!");
		}
	}
	void OnEvent(Zgine::Event& event) override
	{
		//ZG_TRACE("{0}", event);
	}

	virtual void OnImGuiRender() override
	{
		//ImGui::Begin("Hello, ImGui!");
		//ImGui::Text("This is some useful text.");
		//ImGui::End();
	}
};

class Sandbox : public Zgine::Application {
	public:
		Sandbox()
		{
			PushLayer(new ExampleLayer());
		}
		~Sandbox() {};
};

Zgine::Application* Zgine::CreateApplication() {
	return new Sandbox();
};