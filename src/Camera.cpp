#include "Camera.hpp"

#include <cassert>

float static constexpr MATH_PI = 3.14159265f;
float static constexpr MATH_PI_DIV_4 = MATH_PI * 0.25f;
float static constexpr MATH_2_MUL_PI = 2.0f * MATH_PI;

Camera::Camera(int width, int height)
    : position_ {}
    , worldUp_ { 0.0f, 1.0f, 0.0f }
    , vecForward_ {}
    , vecSide_ {}
    , vecUp_ {}
    , center_ { 0.0f, 0.0f, 0.0f }
    , coord_ { 500.0f, 1.25f, 0.0f }
    , viewVolumeWidth_(300.f)
    , zoom_(1.0f)
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
    worldUp_ = -1.0f * CartesianCoord(coord_.phi, coord_.theta + MATH_PI_DIV_4);
    position_ = center_ + coord_.r * CartesianCoord(coord_.phi, coord_.theta);
    vecForward_ = normalize(center_ - position_);
    vecSide_ = normalize(cross(vecForward_, worldUp_));
    vecUp_ = cross(vecSide_, vecForward_);
}

void Camera::SetAspectRatio(float ratio)
{
    aspectRatio_ = ratio;
}

void Camera::SetAspectRatio(int width, int height)
{
    aspectRatio_ = static_cast<float>(width) / static_cast<float>(height);
}

void Camera::SetCenter(float x, float y, float z)
{
    center_ = glm::vec3 { x, y, z };
    UpdateViewCoord();
}

void Camera::SetPhi(float phi)
{
    coord_.theta = RoundGuard(phi);
    UpdateViewCoord();
}

void Camera::SetViewVolumeWidth(float width)
{
    viewVolumeWidth_ = width;
}

void Camera::SetTheta(float theta)
{
    coord_.phi = RoundGuard(theta);
    UpdateViewCoord();
}

glm::vec3& Camera::GetCenter()
{
    return center_;
}

float& Camera::GetZoom()
{
    return zoom_;
}

glm::vec3& Camera::GetVectorUp()
{
    return vecUp_;
}

glm::vec3& Camera::GetVectorSide()
{
    return vecSide_;
}

glm::vec3& Camera::GetVectorForward()
{
    return vecForward_;
}

SphericalCoord& Camera::GetCoordinates()
{
    return coord_;
}

glm::mat4 Camera::ViewMatrix() const
{
    return glm::lookAt(position_, center_, worldUp_);
}

glm::mat4 Camera::ProjectionMatrix() const
{
    assert(aspectRatio_ != 0.0);

    float const v = viewVolumeWidth_ * aspectRatio_ * zoom_;
    float const h = viewVolumeWidth_ * zoom_;

    return glm::ortho(-v, v, -h, h, 0.1f, 1000.f);
}

glm::mat4 Camera::ViewProjectionMatrix() const
{
    return ProjectionMatrix() * ViewMatrix();
}

void Camera::SetProjection(Camera::Projection projection)
{
    projectionType_ = projection;
}

glm::vec3 const Camera::Position() const
{
    return position_;
}

glm::vec3 const Camera::ForwardVector() const
{
    return vecForward_;
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
