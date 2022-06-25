#ifndef CAMERA_H
#define CAMERA_H

#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct SphericalCoord {
    float r;
    float theta;
    float phi;
};

struct Camera {
    struct Basis {
        glm::vec3 forward;
        glm::vec3 sideway;
        glm::vec3 up;
    };

    glm::vec3 position;
    glm::vec3 worldUp;
    glm::vec3 center;
    Basis basis;
    SphericalCoord coord;
    float aspectRatio;
    float volumeSize;
    float zoom;

    explicit Camera(float aspectRatio);
    explicit Camera(int width, int height);
    void UpdateViewCoord();
    glm::mat4 ViewProjectionMatrix() const;
    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;

private:
    glm::vec3 CartesianCoord(float phi, float theta) const;
};

#endif
