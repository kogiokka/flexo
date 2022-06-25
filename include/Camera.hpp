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
    glm::vec3 m_position;
    glm::vec3 m_worldUp;
    glm::vec3 m_vecForward;
    glm::vec3 m_vecSide;
    glm::vec3 m_vecUp;
    glm::vec3 m_center;
    SphericalCoord m_coord;
    float m_aspectRatio;
    float m_viewVolumeWidth;
    float m_zoom;

    std::tuple<int, int, float, float> m_originRotate;
    std::tuple<float, float, glm::vec3> m_originTranslate;

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
    Projection m_projectionType;
    glm::vec3 CartesianCoord(float theta, float phi) const;
    inline float RoundGuard(float radian);
};

#endif
