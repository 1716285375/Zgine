#pragma once

#include <Zgine/Core/Math/MathTypes.h>

namespace Zgine {

/**
 * @brief Camera class for view and projection matrices
 */
class Camera {
public:
    enum class ProjectionType : uint8_t {
        Perspective = 0,
        Orthographic = 1
    };

    Camera() : m_Projection(Math::Matrix4::Identity()) {
        RecalculateViewMatrix();
        RecalculateProjectionMatrix();
    }

    Camera(const Math::Matrix4& projection) : m_Projection(projection) {
        RecalculateViewMatrix();
    }

    const Math::Matrix4& GetProjection() const { return m_Projection; }
    const Math::Matrix4& GetView() const { return m_ViewMatrix; }

    void SetProjection(const Math::Matrix4& projection) { m_Projection = projection; }

    // Projection type
    ProjectionType GetProjectionType() const { return m_ProjectionType; }
    void SetProjectionType(ProjectionType type) {
        m_ProjectionType = type;
        RecalculateProjectionMatrix();
    }

    // Perspective settings
    float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
    void SetPerspectiveFOV(float fov) {
        m_PerspectiveFOV = fov;
        if (m_ProjectionType == ProjectionType::Perspective) RecalculateProjectionMatrix();
    }

    float GetPerspectiveNear() const { return m_PerspectiveNear; }
    void SetPerspectiveNearClip(float nearClip) {
        m_PerspectiveNear = nearClip;
        if (m_ProjectionType == ProjectionType::Perspective) RecalculateProjectionMatrix();
    }

    float GetPerspectiveFar() const { return m_PerspectiveFar; }
    void SetPerspectiveFarClip(float farClip) {
        m_PerspectiveFar = farClip;
        if (m_ProjectionType == ProjectionType::Perspective) RecalculateProjectionMatrix();
    }

    // Orthographic settings
    float GetOrthographicSize() const { return m_OrthographicSize; }
    void SetOrthographicSize(float size) {
        m_OrthographicSize = size;
        if (m_ProjectionType == ProjectionType::Orthographic) RecalculateProjectionMatrix();
    }

    float GetOrthographicNear() const { return m_OrthographicNear; }
    void SetOrthographicNearClip(float nearClip) {
        m_OrthographicNear = nearClip;
        if (m_ProjectionType == ProjectionType::Orthographic) RecalculateProjectionMatrix();
    }

    float GetOrthographicFar() const { return m_OrthographicFar; }
    void SetOrthographicFarClip(float farClip) {
        m_OrthographicFar = farClip;
        if (m_ProjectionType == ProjectionType::Orthographic) RecalculateProjectionMatrix();
    }

    // Position and rotation
    void SetPosition(const Math::Vector3& position) { m_Position = position; RecalculateViewMatrix(); }
    const Math::Vector3& GetPosition() const { return m_Position; }

    void SetRotation(const Math::Vector3& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
    const Math::Vector3& GetRotation() const { return m_Rotation; }

    const Math::Vector3& GetForward() const { return m_Forward; }
    const Math::Vector3& GetRight() const { return m_Right; }
    const Math::Vector3& GetUp() const { return m_Up; }

    // Aspect ratio
    void SetAspectRatio(float aspectRatio) {
        m_AspectRatio = aspectRatio;
        RecalculateProjectionMatrix();
    }
    float GetAspectRatio() const { return m_AspectRatio; }

private:
    void RecalculateViewMatrix();
    void RecalculateProjectionMatrix();

private:
    // Projection
    ProjectionType m_ProjectionType = ProjectionType::Perspective;
    Math::Matrix4 m_Projection = Math::Matrix4::Identity();
    Math::Matrix4 m_ViewMatrix = Math::Matrix4::Identity();

    // Perspective parameters
    float m_PerspectiveFOV = 45.0f;
    float m_PerspectiveNear = 0.1f;
    float m_PerspectiveFar = 1000.0f;

    // Orthographic parameters
    float m_OrthographicSize = 10.0f;
    float m_OrthographicNear = -1.0f;
    float m_OrthographicFar = 1.0f;

    // Common
    float m_AspectRatio = 16.0f / 9.0f;

    // Transform
    Math::Vector3 m_Position = { 0.0f, 0.0f, 0.0f };
    Math::Vector3 m_Rotation = { 0.0f, -90.0f, 0.0f }; // pitch, yaw, roll
    Math::Vector3 m_Forward = { 0.0f, 0.0f, -1.0f };
    Math::Vector3 m_Right = { 1.0f, 0.0f, 0.0f };
    Math::Vector3 m_Up = { 0.0f, 1.0f, 0.0f };
};

} // namespace Zgine
