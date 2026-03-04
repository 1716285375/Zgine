#pragma once

#include <Zgine/Platform/Input.h>
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

namespace Zgine {

    class GLFWInput : public Input {
    public:
        static void SetWindow(GLFWwindow* window) { s_Window = window; }

    protected:
        virtual bool IsKeyPressedImpl(int keycode) override;
        virtual bool IsMouseButtonPressedImpl(int button) override;
        virtual std::pair<float, float> GetMousePositionImpl() override;
        virtual float GetMouseXImpl() override;
        virtual float GetMouseYImpl() override;

    private:
        static GLFWwindow* s_Window;
    };

}
