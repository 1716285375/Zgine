#pragma once

#include <glm/glm.hpp>

namespace Zgine {

	class PerspectiveCamera
	{
	public:
		PerspectiveCamera(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearClip = 0.1f, float farClip = 1000.0f);

		void SetProjection(float fov, float aspectRatio, float nearClip, float farClip);

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		const glm::vec3& GetRotation() const { return m_Rotation; }
		void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		const glm::vec3& GetForward() const { return m_Forward; }
		const glm::vec3& GetRight() const { return m_Right; }
		const glm::vec3& GetUp() const { return m_Up; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		float GetFOV() const { return m_FOV; }
		void SetFOV(float fov) { m_FOV = fov; RecalculateProjectionMatrix(); }

		float GetAspectRatio() const { return m_AspectRatio; }
		void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; RecalculateProjectionMatrix(); }

		float GetNearClip() const { return m_NearClip; }
		void SetNearClip(float nearClip) { m_NearClip = nearClip; RecalculateProjectionMatrix(); }

		float GetFarClip() const { return m_FarClip; }
		void SetFarClip(float farClip) { m_FarClip = farClip; RecalculateProjectionMatrix(); }

		// Camera movement
		void MoveForward(float distance);
		void MoveRight(float distance);
		void MoveUp(float distance);

		// Camera rotation
		void Rotate(float yaw, float pitch);
		void SetYaw(float yaw);
		void SetPitch(float pitch);

	private:
		void RecalculateProjectionMatrix();
		void RecalculateViewMatrix();

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f }; // Yaw, Pitch, Roll

		glm::vec3 m_Forward = { 0.0f, 0.0f, -1.0f };
		glm::vec3 m_Right = { 1.0f, 0.0f, 0.0f };
		glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };

		float m_FOV = 45.0f;
		float m_AspectRatio = 16.0f / 9.0f;
		float m_NearClip = 0.1f;
		float m_FarClip = 1000.0f;
	};

}
