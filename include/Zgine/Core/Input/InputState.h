#pragma once

#include <Zgine/Core/Input/InputCodes.h>
#include <Zgine/Core/Math/Vector2.h>

#include <bitset>
#include <cstddef>

namespace Zgine {

/**
 * @brief Immutable snapshot of keyboard + mouse state for one frame.
 *
 * The platform backend fills an InputState at the start of each frame.
 * Game systems read from it without touching OS APIs.
 *
 * Design goal: polling API — useful for movement, camera, etc.
 * Event-driven input (press/release) is still handled via Core/Events.
 */
struct InputState {
    // --- Keyboard ---
    static constexpr size_t kMaxKeys = 512;
    std::bitset<kMaxKeys> Keys;           ///< current frame: key is held
    std::bitset<kMaxKeys> KeysLastFrame;  ///< previous frame snapshot

    [[nodiscard]] bool IsKeyDown(KeyCode key) const {
        auto index = static_cast<std::size_t>(key);
        return index < kMaxKeys && Keys[index];
    }
    [[nodiscard]] bool IsKeyUp(KeyCode key) const {
        return !IsKeyDown(key);
    }
    [[nodiscard]] bool IsKeyPressed(KeyCode key) const {   // pressed this frame
        auto index = static_cast<std::size_t>(key);
        return index < kMaxKeys && Keys[index] && !KeysLastFrame[index];
    }
    [[nodiscard]] bool IsKeyReleased(KeyCode key) const {  // released this frame
        auto index = static_cast<std::size_t>(key);
        return index < kMaxKeys && !Keys[index] && KeysLastFrame[index];
    }

    // --- Mouse ---
    static constexpr size_t kMaxButtons = 8;
    std::bitset<kMaxButtons> MouseButtons;
    std::bitset<kMaxButtons> MouseButtonsLastFrame;

    Math::Vector2 MousePosition{0.f, 0.f};
    Math::Vector2 MouseDelta{0.f, 0.f};
    float     ScrollDelta = 0.f;

    [[nodiscard]] bool IsMouseButtonDown(MouseButton btn) const {
        auto index = static_cast<std::size_t>(btn);
        return index < kMaxButtons && MouseButtons[index];
    }
    [[nodiscard]] bool IsMouseButtonPressed(MouseButton btn) const {
        auto index = static_cast<std::size_t>(btn);
        return index < kMaxButtons && MouseButtons[index] && !MouseButtonsLastFrame[index];
    }
    [[nodiscard]] bool IsMouseButtonReleased(MouseButton btn) const {
        auto index = static_cast<std::size_t>(btn);
        return index < kMaxButtons && !MouseButtons[index] && MouseButtonsLastFrame[index];
    }
};

} // namespace Zgine
