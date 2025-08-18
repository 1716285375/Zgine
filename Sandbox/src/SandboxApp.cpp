#include "zgpch.h"
#include <Zgine.h>

class ExampleLayer : public Zgine::Layer {
	public:
		ExampleLayer() : Layer("Example") {}
		virtual ~ExampleLayer() {}
		void OnUpdate() override {
			//ZG_INFO("ExampleLayer Update");
		}
		void OnEvent(Zgine::Event& event) override {
			ZG_TRACE("{0}", event);
		}
};;

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