#include "zgpch.h"
#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Zgine {

	PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
	{
		RecalculateProjectionMatrix();
		RecalculateViewMatrix();
	}

	void PerspectiveCamera::SetProjection(float fov, float aspectRatio, float nearClip, float farClip)
	{
		m_FOV = fov;
		m_AspectRatio = aspectRatio;
		m_NearClip = nearClip;
		m_FarClip = farClip;
		RecalculateProjectionMatrix();
	}

	void PerspectiveCamera::RecalculateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

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

	void PerspectiveCamera::MoveForward(float distance)
	{
		m_Position += m_Forward * distance;
		RecalculateViewMatrix();
	}

	void PerspectiveCamera::MoveRight(float distance)
	{
		m_Position += m_Right * distance;
		RecalculateViewMatrix();
	}

	void PerspectiveCamera::MoveUp(float distance)
	{
		m_Position += m_Up * distance;
		RecalculateViewMatrix();
	}

	void PerspectiveCamera::Rotate(float yaw, float pitch)
	{
		m_Rotation.y += yaw;
		m_Rotation.x += pitch;

		// Clamp pitch to prevent camera flipping
		m_Rotation.x = glm::clamp(m_Rotation.x, -89.0f, 89.0f);

		RecalculateViewMatrix();
	}

	void PerspectiveCamera::SetYaw(float yaw)
	{
		m_Rotation.y = yaw;
		RecalculateViewMatrix();
	}

	void PerspectiveCamera::SetPitch(float pitch)
	{
		m_Rotation.x = glm::clamp(pitch, -89.0f, 89.0f);
		RecalculateViewMatrix();
	}

}
