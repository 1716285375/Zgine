#include <Zgine/Scene/Camera/Camera.h>

namespace Zgine {

void Camera::RecalculateViewMatrix() {
    using namespace Math;

    // Extract yaw and pitch from rotation
    float yaw = m_Rotation.y;
    float pitch = m_Rotation.x;

    // Calculate forward vector from yaw/pitch
    m_Forward.x = Cos(DegToRad(yaw)) * Cos(DegToRad(pitch));
    m_Forward.y = Sin(DegToRad(pitch));
    m_Forward.z = Sin(DegToRad(yaw)) * Cos(DegToRad(pitch));
    m_Forward = Normalize(m_Forward);

    // Calculate right and up vectors
    m_Right = Normalize(Cross(m_Forward, Vector3(0.0f, 1.0f, 0.0f)));
    m_Up = Normalize(Cross(m_Right, m_Forward));

    // Build view matrix
    m_ViewMatrix = Matrix4::LookAt(m_Position, m_Position + m_Forward, m_Up);
}

} // namespace Zgine
