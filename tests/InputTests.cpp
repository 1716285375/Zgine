#include <gtest/gtest.h>

#include <Zgine/Core/Input/Input.h>

#include <cstddef>

namespace {
    void SetKey(Zgine::InputState& state, Zgine::KeyCode key, bool down) {
        state.Keys.set(static_cast<std::size_t>(key), down);
    }

    void SetMouseButton(Zgine::InputState& state, Zgine::MouseButton button, bool down) {
        state.MouseButtons.set(static_cast<std::size_t>(button), down);
    }
}

TEST(InputStateTests, InvalidCodesAreIgnored) {
    Zgine::InputState state;

    EXPECT_FALSE(state.IsKeyDown(Zgine::KeyCode::None));
    EXPECT_TRUE(state.IsKeyUp(Zgine::KeyCode::None));
    EXPECT_FALSE(state.IsKeyPressed(Zgine::KeyCode::None));
    EXPECT_FALSE(state.IsKeyReleased(Zgine::KeyCode::None));

    EXPECT_FALSE(state.IsMouseButtonDown(Zgine::MouseButton::None));
    EXPECT_FALSE(state.IsMouseButtonPressed(Zgine::MouseButton::None));
    EXPECT_FALSE(state.IsMouseButtonReleased(Zgine::MouseButton::None));
}

TEST(InputTests, UpdateStateTracksFrameTransitions) {
    Zgine::Input::UpdateState(Zgine::InputState{});

    Zgine::InputState pressed;
    SetKey(pressed, Zgine::KeyCode::W, true);
    SetMouseButton(pressed, Zgine::MouseButton::Right, true);
    pressed.MousePosition = Zgine::Math::Vector2(10.0f, 20.0f);
    pressed.MouseDelta = Zgine::Math::Vector2(1.0f, 2.0f);
    pressed.ScrollDelta = 1.0f;

    Zgine::Input::UpdateState(pressed);

    EXPECT_TRUE(Zgine::Input::IsKeyDown(Zgine::KeyCode::W));
    EXPECT_TRUE(Zgine::Input::IsKeyPressed(Zgine::KeyCode::W));
    EXPECT_TRUE(Zgine::Input::IsMouseButtonDown(Zgine::MouseButton::Right));
    EXPECT_TRUE(Zgine::Input::IsMouseButtonPressed(Zgine::MouseButton::Right));
    EXPECT_EQ(Zgine::Input::GetMousePosition(), Zgine::Math::Vector2(10.0f, 20.0f));
    EXPECT_EQ(Zgine::Input::GetMouseDelta(), Zgine::Math::Vector2(1.0f, 2.0f));
    EXPECT_FLOAT_EQ(Zgine::Input::GetScrollDelta(), 1.0f);

    Zgine::InputState released;
    released.MousePosition = Zgine::Math::Vector2(12.0f, 25.0f);

    Zgine::Input::UpdateState(released);

    EXPECT_FALSE(Zgine::Input::IsKeyDown(Zgine::KeyCode::W));
    EXPECT_TRUE(Zgine::Input::IsKeyReleased(Zgine::KeyCode::W));
    EXPECT_FALSE(Zgine::Input::IsMouseButtonDown(Zgine::MouseButton::Right));
    EXPECT_TRUE(Zgine::Input::IsMouseButtonReleased(Zgine::MouseButton::Right));
}
