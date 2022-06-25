#include "Camera.hpp"

#include <cassert>

float static constexpr MATH_PI = 3.14159265f;
float static constexpr MATH_PI_DIV_4 = MATH_PI * 0.25f;
float static constexpr MATH_2_MUL_PI = 2.0f * MATH_PI;

Camera::Camera(int width, int height)
    : m_position {}
    , m_worldUp { 0.0f, 1.0f, 0.0f }
    , m_vecForward {}
    , m_vecSide {}
    , m_vecUp {}
    , m_center { 0.0f, 0.0f, 0.0f }
    , m_coord { 500.0f, 1.25f, 0.0f }
    , m_viewVolumeWidth(300.f)
    , m_zoom(1.0f)
{
    SetAspectRatio(width, height);
    UpdateViewCoord();
}

Camera::~Camera() { }

glm::vec3 Camera::CartesianCoord(float phi, float theta) const
{
    using namespace std;

    float const sinPhi = sinf(phi);
    float const cosPhi = cosf(phi);
    float const sinTheta = sinf(theta);
    float const cosTheta = cosf(theta);

    return glm::vec3 { sinTheta * cosPhi, cosTheta, -sinTheta * sinPhi };
}

void Camera::UpdateViewCoord()
{
    using namespace glm;

    // Set world-up vector as an offsetted camera-to-origin vector.
    m_worldUp = -1.0f * CartesianCoord(m_coord.phi, m_coord.theta + MATH_PI_DIV_4);
    m_position = m_center + m_coord.r * CartesianCoord(m_coord.phi, m_coord.theta);
    m_vecForward = normalize(m_center - m_position);
    m_vecSide = normalize(cross(m_vecForward, m_worldUp));
    m_vecUp = cross(m_vecSide, m_vecForward);
}

void Camera::SetAspectRatio(float ratio)
{
    m_aspectRatio = ratio;
}

void Camera::SetAspectRatio(int width, int height)
{
    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

void Camera::SetCenter(float x, float y, float z)
{
    m_center = glm::vec3 { x, y, z };
    UpdateViewCoord();
}

void Camera::SetPhi(float phi)
{
    m_coord.theta = RoundGuard(phi);
    UpdateViewCoord();
}

void Camera::SetViewVolumeWidth(float width)
{
    m_viewVolumeWidth = width;
}

void Camera::SetTheta(float theta)
{
    m_coord.phi = RoundGuard(theta);
    UpdateViewCoord();
}

glm::vec3& Camera::GetCenter()
{
    return m_center;
}

float& Camera::GetZoom()
{
    return m_zoom;
}

glm::vec3& Camera::GetVectorUp()
{
    return m_vecUp;
}

glm::vec3& Camera::GetVectorSide()
{
    return m_vecSide;
}

glm::vec3& Camera::GetVectorForward()
{
    return m_vecForward;
}

SphericalCoord& Camera::GetCoordinates()
{
    return m_coord;
}

glm::mat4 Camera::ViewMatrix() const
{
    return glm::lookAt(m_position, m_center, m_worldUp);
}

glm::mat4 Camera::ProjectionMatrix() const
{
    assert(m_aspectRatio != 0.0);

    float const v = m_viewVolumeWidth * m_aspectRatio * m_zoom;
    float const h = m_viewVolumeWidth * m_zoom;

    return glm::ortho(-v, v, -h, h, 0.1f, 1000.f);
}

glm::mat4 Camera::ViewProjectionMatrix() const
{
    return ProjectionMatrix() * ViewMatrix();
}

void Camera::SetProjection(Camera::Projection projection)
{
    m_projectionType = projection;
}

glm::vec3 const Camera::Position() const
{
    return m_position;
}

glm::vec3 const Camera::ForwardVector() const
{
    return m_vecForward;
}

// Restrict both phi and theta within 0 and 360 degrees.
float Camera::RoundGuard(float radian)
{
    if (radian < 0.0f)
        return radian + MATH_2_MUL_PI;
    else if (radian > MATH_2_MUL_PI)
        return radian - MATH_2_MUL_PI;
    else
        return radian;
}
