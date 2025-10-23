#include "zgpch.h"
#include "WindowsInput.h"
#include "Zgine/Application.h"
#include <GLFW/glfw3.h>

namespace Zgine {
	Input* Input::s_Instance = new WindowsInput();

	/**
	 * @brief Platform-specific key press check implementation
	 * @param keycode The key code to check
	 * @return bool True if the key is pressed
	 * @details Uses GLFW to check if a key is currently pressed
	 */
	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	/**
	 * @brief Platform-specific mouse button press check implementation
	 * @param button The mouse button code to check
	 * @return bool True if the button is pressed
	 * @details Uses GLFW to check if a mouse button is currently pressed
	 */
	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state;
	}

	/**
	 * @brief Platform-specific mouse position getter implementation
	 * @return std::pair<float, float> The mouse position as (x, y) coordinates
	 * @details Uses GLFW to get the current mouse cursor position
	 */
	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	/**
	 * @brief Platform-specific mouse X position getter implementation
	 * @return float The mouse X coordinate
	 * @details Uses GLFW to get the current mouse X position
	 */
	float WindowsInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePositionImpl();
		return x;
	}

	/**
	 * @brief Platform-specific mouse Y position getter implementation
	 * @return float The mouse Y coordinate
	 * @details Uses GLFW to get the current mouse Y position
	 */
	float WindowsInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePositionImpl();
		return y;
	}

}
