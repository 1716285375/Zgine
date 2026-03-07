#pragma once

#include <cstdint>

namespace Zgine {

/**
 * @brief Keyboard key codes.
 *
 * Mirror GLFW key codes (same numeric values) so the platform backend
 * can pass them through without conversion.
 */
enum class KeyCode : uint16_t {
    // Printable keys
    Space        = 32,
    Apostrophe   = 39,   // '
    Comma        = 44,   // ,
    Minus        = 45,   // -
    Period       = 46,   // .
    Slash        = 47,   // /
    D0 = 48, D1, D2, D3, D4, D5, D6, D7, D8, D9,
    Semicolon    = 59,   // ;
    Equal        = 61,   // =
    A  = 65, B, C, D, E, F, G, H, I, J, K, L, M,
    N,  O, P, Q, R, S, T, U, V, W, X, Y, Z,
    LeftBracket  = 91,   // [
    Backslash    = 92,   // back-slash
    RightBracket = 93,   // ]
    GraveAccent  = 96,   // `

    // Function keys
    Escape = 256, Enter, Tab, Backspace, Insert, Delete,
    Right, Left, Down, Up,
    PageUp, PageDown, Home, End,
    F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    // Modifiers
    LeftShift = 340, LeftControl, LeftAlt, LeftSuper,
    RightShift, RightControl, RightAlt, RightSuper,

    None = 0xFFFF
};

/** @brief Mouse button codes */
enum class MouseButton : uint8_t {
    Left   = 0,
    Right  = 1,
    Middle = 2,
    None   = 0xFF
};

} // namespace Zgine
