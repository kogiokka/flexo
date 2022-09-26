#include "Camera.hpp"

#include <cassert>

float static constexpr MATH_PI = 3.14159265f;
float static constexpr MATH_PI_DIV_4 = MATH_PI * 0.25f;
float static constexpr MATH_2_MUL_PI = 2.0f * MATH_PI;

void Camera::UpdateViewCoord()
{
    using namespace glm;

    // Set world-up vector as an offsetted camera-to-origin vector.
    worldUp = -1.0f * CartesianCoord(coord.phi, coord.theta + MATH_PI_DIV_4);
    position = center + coord.r * CartesianCoord(coord.phi, coord.theta);
    basis.forward = normalize(center - position);
    basis.sideway = normalize(cross(basis.forward, worldUp));
    basis.up = cross(basis.sideway, basis.forward);
}

glm::mat4 Camera::ViewMatrix() const
{
    return glm::lookAt(position, center, worldUp);
}

glm::mat4 Camera::ProjectionMatrix() const
{
    float const v = volumeSize * 0.5f * zoom;
    float const h = aspectRatio * volumeSize * 0.5f * zoom;
    return glm::ortho(-h, h, -v, v, 0.1f, 1000.f);
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
