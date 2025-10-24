#pragma once

#include <glm/glm.hpp>

namespace Zgine {

	/**
	 * @brief Perspective camera class for 3D rendering
	 * @details This class provides a perspective projection camera
	 *          suitable for 3D rendering with realistic depth perception
	 */
	class PerspectiveCamera
	{
	public:
		/**
		 * @brief Construct a new PerspectiveCamera object
		 * @param fov Field of view in degrees (default: 45.0f)
		 * @param aspectRatio Aspect ratio of the viewport (default: 16.0f/9.0f)
		 * @param nearClip Near clipping plane distance (default: 0.1f)
		 * @param farClip Far clipping plane distance (default: 1000.0f)
		 * @details Creates a perspective camera with the specified projection parameters
		 */
		PerspectiveCamera(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearClip = 0.1f, float farClip = 1000.0f);

		/**
		 * @brief Set the projection matrix
		 * @param fov Field of view in degrees
		 * @param aspectRatio Aspect ratio of the viewport
		 * @param nearClip Near clipping plane distance
		 * @param farClip Far clipping plane distance
		 * @details Updates the projection matrix with new parameters
		 */
		void SetProjection(float fov, float aspectRatio, float nearClip, float farClip);

		/**
		 * @brief Get the camera position
		 * @return const glm::vec3& Reference to the camera position
		 */
		const glm::vec3& GetPosition() const { return m_Position; }
		
		/**
		 * @brief Set the camera position
		 * @param position The new camera position
		 * @details Updates the camera position and recalculates the view matrix
		 */
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		/**
		 * @brief Get the camera rotation
		 * @return const glm::vec3& Reference to the camera rotation (yaw, pitch, roll)
		 */
		const glm::vec3& GetRotation() const { return m_Rotation; }
		
		/**
		 * @brief Set the camera rotation
		 * @param rotation The new camera rotation (yaw, pitch, roll)
		 * @details Updates the camera rotation and recalculates the view matrix
		 */
		void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		/**
		 * @brief Get the forward direction vector
		 * @return const glm::vec3& Reference to the forward direction
		 */
		const glm::vec3& GetForward() const { return m_Forward; }
		
		/**
		 * @brief Get the right direction vector
		 * @return const glm::vec3& Reference to the right direction
		 */
		const glm::vec3& GetRight() const { return m_Right; }
		
		/**
		 * @brief Get the up direction vector
		 * @return const glm::vec3& Reference to the up direction
		 */
		const glm::vec3& GetUp() const { return m_Up; }

		/**
		 * @brief Get the projection matrix
		 * @return const glm::mat4& Reference to the projection matrix
		 */
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		
		/**
		 * @brief Get the view matrix
		 * @return const glm::mat4& Reference to the view matrix
		 */
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		
		/**
		 * @brief Get the combined view-projection matrix
		 * @return const glm::mat4& Reference to the view-projection matrix
		 */
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		/**
		 * @brief Get the field of view
		 * @return float The field of view in degrees
		 */
		float GetFOV() const { return m_FOV; }
		
		/**
		 * @brief Set the field of view
		 * @param fov The new field of view in degrees
		 * @details Updates the FOV and recalculates the projection matrix
		 */
		void SetFOV(float fov) { m_FOV = fov; RecalculateProjectionMatrix(); }

		/**
		 * @brief Get the aspect ratio
		 * @return float The aspect ratio
		 */
		float GetAspectRatio() const { return m_AspectRatio; }
		
		/**
		 * @brief Set the aspect ratio
		 * @param aspectRatio The new aspect ratio
		 * @details Updates the aspect ratio and recalculates the projection matrix
		 */
		void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; RecalculateProjectionMatrix(); }

		/**
		 * @brief Get the near clipping plane distance
		 * @return float The near clipping plane distance
		 */
		float GetNearClip() const { return m_NearClip; }
		
		/**
		 * @brief Set the near clipping plane distance
		 * @param nearClip The new near clipping plane distance
		 * @details Updates the near clip and recalculates the projection matrix
		 */
		void SetNearClip(float nearClip) { m_NearClip = nearClip; RecalculateProjectionMatrix(); }

		/**
		 * @brief Get the far clipping plane distance
		 * @return float The far clipping plane distance
		 */
		float GetFarClip() const { return m_FarClip; }
		
		/**
		 * @brief Set the far clipping plane distance
		 * @param farClip The new far clipping plane distance
		 * @details Updates the far clip and recalculates the projection matrix
		 */
		void SetFarClip(float farClip) { m_FarClip = farClip; RecalculateProjectionMatrix(); }

		/**
		 * @brief Move the camera forward
		 * @param distance The distance to move forward
		 * @details Moves the camera along its forward direction
		 */
		void MoveForward(float distance);
		
		/**
		 * @brief Move the camera right
		 * @param distance The distance to move right
		 * @details Moves the camera along its right direction
		 */
		void MoveRight(float distance);
		
		/**
		 * @brief Move the camera up
		 * @param distance The distance to move up
		 * @details Moves the camera along its up direction
		 */
		void MoveUp(float distance);

		/**
		 * @brief Rotate the camera
		 * @param yaw The yaw rotation in degrees
		 * @param pitch The pitch rotation in degrees
		 * @details Rotates the camera by the specified yaw and pitch angles
		 */
		void Rotate(float yaw, float pitch);
		
		/**
		 * @brief Set the yaw rotation
		 * @param yaw The new yaw rotation in degrees
		 * @details Sets the camera's yaw rotation
		 */
		void SetYaw(float yaw);
		
		/**
		 * @brief Set the pitch rotation
		 * @param pitch The new pitch rotation in degrees
		 * @details Sets the camera's pitch rotation
		 */
		void SetPitch(float pitch);
		
		/**
		 * @brief Get the near clipping plane distance
		 * @return float The near clipping plane distance
		 */
		float GetNearPlane() const { return m_NearClip; }
		
		/**
		 * @brief Get the far clipping plane distance
		 * @return float The far clipping plane distance
		 */
		float GetFarPlane() const { return m_FarClip; }

	private:
		/**
		 * @brief Recalculate the projection matrix
		 * @details Updates the projection matrix based on current FOV, aspect ratio, and clipping planes
		 */
		void RecalculateProjectionMatrix();
		
		/**
		 * @brief Recalculate the view matrix
		 * @details Updates the view matrix based on current position and rotation
		 */
		void RecalculateViewMatrix();

	private:
		glm::mat4 m_ProjectionMatrix;      ///< Projection matrix
		glm::mat4 m_ViewMatrix;            ///< View matrix
		glm::mat4 m_ViewProjectionMatrix;  ///< Combined view-projection matrix

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };  ///< Camera position
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };  ///< Camera rotation (yaw, pitch, roll)

		glm::vec3 m_Forward = { 0.0f, 0.0f, -1.0f };  ///< Forward direction vector
		glm::vec3 m_Right = { 1.0f, 0.0f, 0.0f };     ///< Right direction vector
		glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };        ///< Up direction vector

		float m_FOV = 45.0f;                    ///< Field of view in degrees
		float m_AspectRatio = 16.0f / 9.0f;    ///< Aspect ratio
		float m_NearClip = 0.1f;               ///< Near clipping plane distance
		float m_FarClip = 1000.0f;             ///< Far clipping plane distance
	};

}
