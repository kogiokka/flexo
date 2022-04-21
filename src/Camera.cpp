#include "Camera.hpp"

#include <cassert>

float static constexpr MATH_PI = 3.14159265f;
float static constexpr MATH_PI_DIV_4 = MATH_PI * 0.25f;
float static constexpr MATH_2_MUL_PI = 2.0f * MATH_PI;

Camera::Camera(int width, int height, Camera::Projection projectionType)
    : position_ {}
    , worldUp_ { 0.0f, 1.0f, 0.0f }
    , vecForward_ {}
    , vecSide_ {}
    , vecUp_ {}
    , center_ { 0.0f, 0.0f, 0.0f }
    , phi_(0.0f)
    , theta_(1.25f)
    , radius_(100.0f)
    , viewVolumeWidth_(100.f)
    , rateMove_(0.4f)
    , rateRotate_(0.005f)
    , zoom_(0.8f)
    , dirHorizontal_(1)
    , projectionType_(projectionType)
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
    worldUp_ = -1.0f * CartesianCoord(phi_, theta_ + MATH_PI_DIV_4);
    position_ = center_ + radius_ * CartesianCoord(phi_, theta_);
    vecForward_ = normalize(center_ - position_);
    vecSide_ = normalize(cross(vecForward_, worldUp_));
    vecUp_ = cross(vecSide_, vecForward_);
}

void Camera::InitDragRotation(int x, int y)
{
    // Change rotating direction if the camera passes through the polars.
    // Theta is guaranteed to be on either [0, pi] or (pi, 2 * pi).
    if (theta_ > MATH_PI)
        dirHorizontal_ = -1;
    else
        dirHorizontal_ = 1;

    originRotate_ = std::make_tuple(x, y, phi_, theta_);
}

void Camera::DragRotation(int x, int y)
{
    auto const& [oX, oY, oPhi, oTheta] = originRotate_;

    phi_ = RoundGuard(dirHorizontal_ * -(x - oX) * rateRotate_ + oPhi);
    theta_ = RoundGuard(-(y - oY) * rateRotate_ + oTheta);

    UpdateViewCoord();
}

void Camera::InitDragTranslation(int x, int y)
{
    originTranslate_ = std::make_tuple(x, y, center_);
}

void Camera::DragTranslation(int x, int y)
{
    auto const& [oX, oY, oTarget] = originTranslate_;

    center_ = oTarget + (-(x - oX) * vecSide_ + (y - oY) * vecUp_) * rateMove_ * zoom_;

    UpdateViewCoord();
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
    theta_ = RoundGuard(phi);
    UpdateViewCoord();
}

void Camera::SetTheta(float theta)
{
    phi_ = RoundGuard(theta);
    UpdateViewCoord();
}

void Camera::WheelZoom(int direction)
{
    assert(direction == 1 || direction == -1);

    switch (projectionType_) {
    case Projection::Orthographic: {
        float const tmp_zoom = zoom_ + direction * -0.02f;
        constexpr float min = 0.01f;
        constexpr float max = 1.0f;

        if (tmp_zoom < min) {
            zoom_ = min;
        } else if (tmp_zoom >= max) {
            zoom_ = max;
        } else {
            zoom_ = tmp_zoom;
        }
    } break;
    case Projection::Perspective: {
        radius_ += direction * -2.0f;
        UpdateViewCoord();
    } break;
    }
}

glm::mat4 Camera::ViewMatrix() const
{
    return glm::lookAt(position_, center_, worldUp_);
}

glm::mat4 Camera::ProjectionMatrix() const
{
    assert(aspectRatio_ != 0.0);
    glm::mat4 mat;
    switch (projectionType_) {
    case Projection::Orthographic: {
        float const v = viewVolumeWidth_ * aspectRatio_ * zoom_;
        float const h = viewVolumeWidth_ * zoom_;
        mat = glm::ortho(-v, v, -h, h, 0.1f, 1000.f);
    } break;
    case Projection::Perspective: {
        mat = glm::perspective(glm::radians(45.0f), aspectRatio_, 0.1f, 2000.0f);
    } break;
    }

    return mat;
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

void Camera::Moving(Camera::Translate direction)
{
    float const speed = 10.0f * rateMove_ * zoom_;
    switch (direction) {
    case Translate::Up:
        center_ += -vecUp_ * speed;
        break;
    case Translate::Down:
        center_ += vecUp_ * speed;
        break;
    case Translate::Right:
        center_ += -vecSide_ * speed;
        break;
    case Translate::Left:
        center_ += vecSide_ * speed;
        break;
    case Translate::Forward:
        center_ += vecForward_ * speed;
        break;
    case Translate::Backward:
        center_ += -vecForward_ * speed;
        break;
    }

    UpdateViewCoord();
}

void Camera::Turning(Camera::Rotate direction)
{
    switch (direction) {
    case Rotate::Clockwise:
        phi_ = phi_ + 0.1f;
        break;
    case Rotate::CounterClockwise:
        phi_ = phi_ - 0.1f;
        break;
    case Rotate::PitchUp:
        theta_ = theta_ - 0.1f;
        break;
    case Rotate::PitchDown:
        theta_ = theta_ + 0.1f;
        break;
    }

    theta_ = RoundGuard(theta_);
    phi_ = RoundGuard(phi_);
    UpdateViewCoord();
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
