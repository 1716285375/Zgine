#include <Zgine/Platform/Input.h>

// Assuming Windows for this build environment to unblock
#include "Backend/GLFW/GLFWInput.h"

namespace Zgine {

    std::unique_ptr<Input> Input::s_Instance = std::make_unique<GLFWInput>();

    bool Input::IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
    bool Input::IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
    std::pair<float, float> Input::GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
    float Input::GetMouseX() { return s_Instance->GetMouseXImpl(); }
    float Input::GetMouseY() { return s_Instance->GetMouseYImpl(); }

}
