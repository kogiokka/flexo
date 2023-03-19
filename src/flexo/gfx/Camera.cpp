#include "gfx/Camera.hpp"

#include <cassert>

float static constexpr MATH_PI = 3.14159265f;
float static constexpr MATH_PI_DIV_4 = MATH_PI * 0.25f;

Camera::Camera()
{
    position = { 0.0f, 0.0f, 0.0f };
    basis = {};
    worldUp = { 0.0f, 1.0f, 0.0f };
    center = { 0.0f, 0.0f, 0.0f };
    volumeSize = 10.0f;
    zoom = 1.0f;

    orthoCoord = std::make_shared<SphericalCoord>(1000.0f, glm::radians(50.0f), glm::radians(-50.0f));
    perspCoord = std::make_shared<SphericalCoord>(10.0f, glm::radians(50.0f), glm::radians(-50.0f));

    SetProjectionMode(Camera::ProjectionMode::Perspective);
    UpdateViewCoord();
}

void Camera::UpdateViewCoord()
{
    using namespace glm;

    // Set world-up vector as an offsetted camera-to-origin vector.
    auto const crd = coord.lock();
    worldUp = -1.0f * CartesianCoord(crd->phi, crd->theta + MATH_PI_DIV_4);
    position = center + crd->r * CartesianCoord(crd->phi, crd->theta);
    basis.forward = normalize(center - position);
    basis.sideway = normalize(cross(basis.forward, worldUp));
    basis.up = cross(basis.sideway, basis.forward);
}

glm::mat4 Camera::ViewMatrix() const
{
    return glm::lookAt(position, center, worldUp);
}

void Camera::SetProjectionMode(ProjectionMode mode)
{
    projmode = mode;
    switch (mode) {
    case Perspective:
        coord = perspCoord;
        break;
    case Orthogonal:
        coord = orthoCoord;
        break;
    }
    UpdateViewCoord();
}

void Camera::Zoom(int diff)
{
    perspCoord->r += diff * (perspCoord->r / 10.0f);
    zoom += diff * (zoom / 10.0f);
    UpdateViewCoord();
}

glm::mat4 Camera::ProjectionMatrix() const
{
    switch (projmode) {
    case Perspective: {
        return glm::perspective(glm::radians(45.0f), aspectRatio, 0.01f, 2000.f);
    } break;
    case Orthogonal: {
        float const v = volumeSize * 0.5f * zoom;
        float const h = aspectRatio * volumeSize * 0.5f * zoom;
        return glm::ortho(-h, h, -v, v, 0.01f, 2000.f);
    } break;
    }
}

glm::mat4 Camera::ViewProjectionMatrix() const
{
    return ProjectionMatrix() * ViewMatrix();
}

glm::vec3 Camera::CartesianCoord(float phi, float theta) const
{
    using namespace std;

    float const sinPhi = sinf(phi);
    float const cosPhi = cosf(phi);
    float const sinTheta = sinf(theta);
    float const cosTheta = cosf(theta);

    return glm::vec3 { sinTheta * cosPhi, sinTheta * sinPhi, cosTheta };
}
