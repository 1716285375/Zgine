#include <Zgine/Core/Input/Input.h>

namespace Zgine {

InputState Input::s_Current{};

bool Input::IsKeyDown    (KeyCode key) { return s_Current.IsKeyDown(key);     }
bool Input::IsKeyUp      (KeyCode key) { return s_Current.IsKeyUp(key);       }
bool Input::IsKeyPressed (KeyCode key) { return s_Current.IsKeyPressed(key);  }
bool Input::IsKeyReleased(KeyCode key) { return s_Current.IsKeyReleased(key); }

bool Input::IsMouseButtonDown    (MouseButton btn) { return s_Current.IsMouseButtonDown(btn);     }
bool Input::IsMouseButtonPressed (MouseButton btn) { return s_Current.IsMouseButtonPressed(btn);  }
bool Input::IsMouseButtonReleased(MouseButton btn) { return s_Current.IsMouseButtonReleased(btn); }

Math::Vector2 Input::GetMousePosition() { return s_Current.MousePosition; }
Math::Vector2 Input::GetMouseDelta()    { return s_Current.MouseDelta;    }
float     Input::GetScrollDelta()   { return s_Current.ScrollDelta;   }

void Input::UpdateState(const InputState& newState) {
    s_Current.KeysLastFrame          = s_Current.Keys;
    s_Current.MouseButtonsLastFrame  = s_Current.MouseButtons;
    s_Current.Keys                   = newState.Keys;
    s_Current.MouseButtons           = newState.MouseButtons;
    s_Current.MousePosition          = newState.MousePosition;
    s_Current.MouseDelta             = newState.MouseDelta;
    s_Current.ScrollDelta            = newState.ScrollDelta;
}

const InputState& Input::GetState() { return s_Current; }

} // namespace Zgine
