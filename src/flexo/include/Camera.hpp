#ifndef CAMERA_H
#define CAMERA_H

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct SphericalCoord {
    SphericalCoord(float radius, float theta, float phi)
        : r(radius)
        , theta(theta)
        , phi(phi)
    {
    }
    float r;
    float theta;
    float phi;
};

struct Camera {
    enum ProjectionMode {
        Perspective,
        Orthogonal,
    };
    struct Basis {
        glm::vec3 forward;
        glm::vec3 sideway;
        glm::vec3 up;
    };

    Camera();
    glm::vec3 position;
    glm::vec3 worldUp;
    glm::vec3 center;
    Basis basis;
    std::weak_ptr<SphericalCoord> coord;
    std::shared_ptr<SphericalCoord> orthoCoord;
    std::shared_ptr<SphericalCoord> perspCoord;
    float aspectRatio;
    float volumeSize;
    float zoom;

    void SetProjectionMode(ProjectionMode mode);
    void Zoom(int direction);
    void UpdateViewCoord();
    glm::mat4 ViewProjectionMatrix() const;
    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

private:
    glm::vec3 CartesianCoord(float phi, float theta) const;

    ProjectionMode projmode;
};

#endif
