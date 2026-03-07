#pragma once

#include <Zgine/Core/Input/InputState.h>
#include <Zgine/Core/Input/InputCodes.h>
#include <Zgine/Core/Math/Vector2.h>

namespace Zgine {

/**
 * @brief Global polling-style input API.
 *
 * The Input singleton is updated once per frame by the platform layer
 * (Window/GLFW backend) before any game logic runs.
 *
 * Usage:
 *   if (Input::IsKeyDown(KeyCode::W)) camera.MoveForward(speed * dt);
 */
class Input {
public:
    // --- Keyboard ---
    [[nodiscard]] static bool IsKeyDown    (KeyCode key);
    [[nodiscard]] static bool IsKeyUp      (KeyCode key);
    [[nodiscard]] static bool IsKeyPressed (KeyCode key);   // true only on first frame
    [[nodiscard]] static bool IsKeyReleased(KeyCode key);

    // --- Mouse ---
    [[nodiscard]] static bool      IsMouseButtonDown    (MouseButton btn);
    [[nodiscard]] static bool      IsMouseButtonPressed (MouseButton btn);
    [[nodiscard]] static bool      IsMouseButtonReleased(MouseButton btn);
    [[nodiscard]] static Math::Vector2 GetMousePosition();
    [[nodiscard]] static Math::Vector2 GetMouseDelta();
    [[nodiscard]] static float     GetScrollDelta();

    // --- Platform backend (not for game code) ---
    static void UpdateState(const InputState& newState);
    [[nodiscard]] static const InputState& GetState();

private:
    static InputState s_Current;
};

} // namespace Zgine
