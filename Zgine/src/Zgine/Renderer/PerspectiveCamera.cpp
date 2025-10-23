#include "zgpch.h"
#include "PerspectiveCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Zgine {

	/**
	 * @brief Construct a new PerspectiveCamera object
	 * @param fov Field of view in degrees
	 * @param aspectRatio Aspect ratio of the viewport
	 * @param nearClip Near clipping plane distance
	 * @param farClip Far clipping plane distance
	 * @details Creates a perspective camera with the specified projection parameters
	 */
	PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
	{
		RecalculateProjectionMatrix();
		RecalculateViewMatrix();
	}

	/**
	 * @brief Set the projection matrix
	 * @param fov Field of view in degrees
	 * @param aspectRatio Aspect ratio of the viewport
	 * @param nearClip Near clipping plane distance
	 * @param farClip Far clipping plane distance
	 * @details Updates the projection matrix with new parameters
	 */
	void PerspectiveCamera::SetProjection(float fov, float aspectRatio, float nearClip, float farClip)
	{
		m_FOV = fov;
		m_AspectRatio = aspectRatio;
		m_NearClip = nearClip;
		m_FarClip = farClip;
		RecalculateProjectionMatrix();
	}

	/**
	 * @brief Recalculate the projection matrix
	 * @details Updates the projection matrix based on current FOV, aspect ratio, and clipping planes
	 */
	void PerspectiveCamera::RecalculateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	/**
	 * @brief Recalculate the view matrix
	 * @details Updates the view matrix based on current position and rotation
	 */
	void PerspectiveCamera::RecalculateViewMatrix()
	{
		// Convert rotation from degrees to radians
		float yaw = glm::radians(m_Rotation.y);
		float pitch = glm::radians(m_Rotation.x);
		float roll = glm::radians(m_Rotation.z);

		// Calculate forward vector from yaw and pitch
		m_Forward.x = cos(yaw) * cos(pitch);
		m_Forward.y = sin(pitch);
		m_Forward.z = sin(yaw) * cos(pitch);
		m_Forward = glm::normalize(m_Forward);

		// Calculate right vector
		m_Right = glm::normalize(glm::cross(m_Forward, glm::vec3(0.0f, 1.0f, 0.0f)));

		// Calculate up vector
		m_Up = glm::normalize(glm::cross(m_Right, m_Forward));

		// Apply roll rotation to right and up vectors
		if (roll != 0.0f)
		{
			float cosRoll = cos(roll);
			float sinRoll = sin(roll);
			
			glm::vec3 right = m_Right;
			glm::vec3 up = m_Up;
			
			m_Right = right * cosRoll + up * sinRoll;
			m_Up = -right * sinRoll + up * cosRoll;
		}

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	/**
	 * @brief Move the camera forward
	 * @param distance The distance to move forward
	 * @details Moves the camera along its forward direction
	 */
	void PerspectiveCamera::MoveForward(float distance)
	{
		m_Position += m_Forward * distance;
		RecalculateViewMatrix();
	}

	/**
	 * @brief Move the camera right
	 * @param distance The distance to move right
	 * @details Moves the camera along its right direction
	 */
	void PerspectiveCamera::MoveRight(float distance)
	{
		m_Position += m_Right * distance;
		RecalculateViewMatrix();
	}

	/**
	 * @brief Move the camera up
	 * @param distance The distance to move up
	 * @details Moves the camera along its up direction
	 */
	void PerspectiveCamera::MoveUp(float distance)
	{
		m_Position += m_Up * distance;
		RecalculateViewMatrix();
	}

	/**
	 * @brief Rotate the camera
	 * @param yaw The yaw rotation in degrees
	 * @param pitch The pitch rotation in degrees
	 * @details Rotates the camera by the specified yaw and pitch angles
	 */
	void PerspectiveCamera::Rotate(float yaw, float pitch)
	{
		m_Rotation.y += yaw;
		m_Rotation.x += pitch;

		// Clamp pitch to prevent camera flipping
		m_Rotation.x = glm::clamp(m_Rotation.x, -89.0f, 89.0f);

		RecalculateViewMatrix();
	}

	/**
	 * @brief Set the yaw rotation
	 * @param yaw The new yaw rotation in degrees
	 * @details Sets the camera's yaw rotation
	 */
	void PerspectiveCamera::SetYaw(float yaw)
	{
		m_Rotation.y = yaw;
		RecalculateViewMatrix();
	}

	/**
	 * @brief Set the pitch rotation
	 * @param pitch The new pitch rotation in degrees
	 * @details Sets the camera's pitch rotation
	 */
	void PerspectiveCamera::SetPitch(float pitch)
	{
		m_Rotation.x = glm::clamp(pitch, -89.0f, 89.0f);
		RecalculateViewMatrix();
	}

}
