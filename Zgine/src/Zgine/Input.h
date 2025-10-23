#pragma once

namespace Zgine {

	/**
	 * @brief Abstract input handling class
	 * @details This class provides a platform-independent interface for
	 *          handling keyboard and mouse input
	 */
	class ZG_API Input
	{
	public:
		/**
		 * @brief Check if a key is currently pressed
		 * @param keycode The key code to check
		 * @return bool True if the key is pressed
		 */
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		
		/**
		 * @brief Check if a mouse button is currently pressed
		 * @param button The mouse button code to check
		 * @return bool True if the button is pressed
		 */
		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		
		/**
		 * @brief Get the current mouse position
		 * @return std::pair<float, float> The mouse position as (x, y) coordinates
		 */
		inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
		
		/**
		 * @brief Get the current mouse X position
		 * @return float The mouse X coordinate
		 */
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		
		/**
		 * @brief Get the current mouse Y position
		 * @return float The mouse Y coordinate
		 */
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
	
	protected:
		/**
		 * @brief Platform-specific key press check implementation
		 * @param keycode The key code to check
		 * @return bool True if the key is pressed
		 */
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		
		/**
		 * @brief Platform-specific mouse button press check implementation
		 * @param button The mouse button code to check
		 * @return bool True if the button is pressed
		 */
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		
		/**
		 * @brief Platform-specific mouse position getter implementation
		 * @return std::pair<float, float> The mouse position as (x, y) coordinates
		 */
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		
		/**
		 * @brief Platform-specific mouse X position getter implementation
		 * @return float The mouse X coordinate
		 */
		virtual float GetMouseXImpl() = 0;
		
		/**
		 * @brief Platform-specific mouse Y position getter implementation
		 * @return float The mouse Y coordinate
		 */
		virtual float GetMouseYImpl() = 0;
		
	private:
		static Input* s_Instance;  ///< Singleton instance of the input handler
	};
}