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

class Camera
{
    glm::vec3 position_;
    glm::vec3 worldUp_;
    glm::vec3 vecForward_;
    glm::vec3 vecSide_;
    glm::vec3 vecUp_;
    glm::vec3 center_;
    SphericalCoord coord_;
    float aspectRatio_;
    float viewVolumeWidth_;
    float zoom_;

    std::tuple<int, int, float, float> originRotate_;
    std::tuple<float, float, glm::vec3> originTranslate_;

public:
    enum class Projection {
        Orthographic,
        Perspective,
    };
    Camera(int width, int height);
    ~Camera();
    void SetAspectRatio(float ratio);
    void SetAspectRatio(int width, int height);
    void SetCenter(float x, float y, float z);
    void SetTheta(float theta);
    void SetPhi(float phi);
    void SetViewVolumeWidth(float width);
    glm::vec3& GetCenter();
    float& GetZoom();
    glm::vec3& GetVectorUp();
    glm::vec3& GetVectorSide();
    glm::vec3& GetVectorForward();
    SphericalCoord& GetCoordinates();
    void UpdateViewCoord();
    void SetProjection(Camera::Projection projection);
    glm::mat4 ViewProjectionMatrix() const;
    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;
    glm::vec3 const Position() const;
    glm::vec3 const ForwardVector() const;

private:
    Projection projectionType_;
    glm::vec3 CartesianCoord(float theta, float phi) const;
    inline float RoundGuard(float radian);
};

#endif
