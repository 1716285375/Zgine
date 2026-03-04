#include "GLFWInput.h"
#include <GLFW/glfw3.h>

namespace Zgine {

    GLFWwindow* GLFWInput::s_Window = nullptr;

    bool GLFWInput::IsKeyPressedImpl(int keycode) {
        if (!s_Window) return false;
        auto state = glfwGetKey(s_Window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool GLFWInput::IsMouseButtonPressedImpl(int button) {
        if (!s_Window) return false;
        auto state = glfwGetMouseButton(s_Window, button);
        return state == GLFW_PRESS;
    }

    std::pair<float, float> GLFWInput::GetMousePositionImpl() {
        if (!s_Window) return { 0.0f, 0.0f };
        double xpos, ypos;
        glfwGetCursorPos(s_Window, &xpos, &ypos);
        return { (float)xpos, (float)ypos };
    }

    float GLFWInput::GetMouseXImpl() {
        auto [x, y] = GetMousePositionImpl();
        return x;
    }

    float GLFWInput::GetMouseYImpl() {
        auto [x, y] = GetMousePositionImpl();
        return y;
    }

}
