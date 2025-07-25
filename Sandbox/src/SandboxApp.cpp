#include <Zgine.h>

class Sandbox : public Zgine::Application {
	public:
		Sandbox() {};
		~Sandbox() {};
};

Zgine::Application* Zgine::CreateApplication() {
	return new Sandbox();
};