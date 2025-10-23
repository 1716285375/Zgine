#pragma once
#include "Zgine/Input.h"

namespace Zgine {

	/**
	 * @brief Windows platform specific input implementation
	 * @details This class implements the Input interface using GLFW
	 *          for Windows platform input handling
	 */
	class WindowsInput : public Input
	{
	protected:
		/**
		 * @brief Platform-specific key press check implementation
		 * @param keycode The key code to check
		 * @return bool True if the key is pressed
		 * @details Uses GLFW to check if a key is currently pressed
		 */
		virtual bool IsKeyPressedImpl(int keycode) override;

		/**
		 * @brief Platform-specific mouse button press check implementation
		 * @param button The mouse button code to check
		 * @return bool True if the button is pressed
		 * @details Uses GLFW to check if a mouse button is currently pressed
		 */
		virtual bool IsMouseButtonPressedImpl(int button) override;

		/**
		 * @brief Platform-specific mouse position getter implementation
		 * @return std::pair<float, float> The mouse position as (x, y) coordinates
		 * @details Uses GLFW to get the current mouse cursor position
		 */
		virtual std::pair<float, float> GetMousePositionImpl() override;

		/**
		 * @brief Platform-specific mouse X position getter implementation
		 * @return float The mouse X coordinate
		 * @details Uses GLFW to get the current mouse X position
		 */
		virtual float GetMouseXImpl() override;

		/**
		 * @brief Platform-specific mouse Y position getter implementation
		 * @return float The mouse Y coordinate
		 * @details Uses GLFW to get the current mouse Y position
		 */
		virtual float GetMouseYImpl() override;
	};

}


