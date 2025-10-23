#pragma once

#include <glm/glm.hpp>

namespace Zgine {

	/**
	 * @brief Orthographic camera class for 2D rendering
	 * @details This class provides an orthographic projection camera
	 *          suitable for 2D rendering and UI elements
	 */
	class OrthographicCamera
	{
	public:
		/**
		 * @brief Construct a new OrthographicCamera object
		 * @param left Left boundary of the projection
		 * @param right Right boundary of the projection
		 * @param bottom Bottom boundary of the projection
		 * @param top Top boundary of the projection
		 * @details Creates an orthographic camera with the specified projection bounds
		 */
		OrthographicCamera(float left, float right, float bottom, float top);

		/**
		 * @brief Set the projection matrix
		 * @param left Left boundary of the projection
		 * @param right Right boundary of the projection
		 * @param bottom Bottom boundary of the projection
		 * @param top Top boundary of the projection
		 * @details Updates the projection matrix with new bounds
		 */
		void SetProjection(float left, float right, float bottom, float top);

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
		 * @return float The camera rotation in degrees
		 */
		float GetRotation() const { return m_Rotation; }
		
		/**
		 * @brief Set the camera rotation
		 * @param rotation The new camera rotation in degrees
		 * @details Updates the camera rotation and recalculates the view matrix
		 */
		void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

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

	private:
		/**
		 * @brief Recalculate the view matrix
		 * @details Updates the view matrix based on current position and rotation
		 */
		void RecalculateViewMatrix();

	private:
		glm::mat4 m_ProjectionMatrix;      ///< Projection matrix
		glm::mat4 m_ViewMatrix;            ///< View matrix
		glm::mat4 m_ViewProjectionMatrix;  ///< Combined view-projection matrix

		glm::vec3 m_Position;              ///< Camera position
		float m_Rotation = 0.0f;            ///< Camera rotation in degrees
	};

}