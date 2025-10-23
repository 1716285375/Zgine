#include "zgpch.h"
#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Zgine {

	/**
	 * @brief Construct a new OrthographicCamera object
	 * @param left Left boundary of the projection
	 * @param right Right boundary of the projection
	 * @param bottom Bottom boundary of the projection
	 * @param top Top boundary of the projection
	 * @details Creates an orthographic camera with the specified projection bounds
	 */
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	/**
	 * @brief Set the projection matrix
	 * @param left Left boundary of the projection
	 * @param right Right boundary of the projection
	 * @param bottom Bottom boundary of the projection
	 * @param top Top boundary of the projection
	 * @details Updates the projection matrix with new bounds
	 */
	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	/**
	 * @brief Recalculate the view matrix
	 * @details Updates the view matrix based on current position and rotation
	 */
	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * 
			glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}