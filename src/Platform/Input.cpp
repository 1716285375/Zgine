#include <Zgine/Platform/Input.h>

// Assuming Windows for this build environment to unblock
#include "Backend/GLFW/GLFWInput.h"

namespace Zgine
{

    std::unique_ptr<Input> Input::s_Instance = std::make_unique<GLFWInput>();

    [[nodiscard]] bool Input::IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
    [[nodiscard]] bool Input::IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
    [[nodiscard]] std::pair<float, float> Input::GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
    [[nodiscard]] float Input::GetMouseX() { return s_Instance->GetMouseXImpl(); }
    [[nodiscard]] float Input::GetMouseY() { return s_Instance->GetMouseYImpl(); }

}
