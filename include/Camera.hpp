#ifndef CAMERA_H
#define CAMERA_H

#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
    glm::vec3 position_;
    glm::vec3 worldUp_;
    glm::vec3 vecForward_;
    glm::vec3 vecSide_;
    glm::vec3 vecUp_;
    glm::vec3 center_;
    float phi_;
    float theta_;
    float radius_;
    float aspectRatio_;
    float viewVolumeWidth_;
    float rateMove_;
    float rateRotate_;
    float zoom_;
    int dirHorizontal_;

    std::tuple<int, int, float, float> originRotate_;
    std::tuple<float, float, glm::vec3> originTranslate_;

public:
    enum class Translate {
        Up,
        Down,
        Left,
        Right,
        Forward,
        Backward,
    };
    enum class Rotate {
        Clockwise,
        CounterClockwise,
        PitchUp,
        PitchDown,
    };
    enum class Projection {
        Orthographic,
        Perspective,
    };
    Camera(int width, int height, Projection projectionType = Camera::Projection::Orthographic);
    ~Camera();
    void SetAspectRatio(float ratio);
    void SetAspectRatio(int width, int height);
    void SetCenter(float x, float y, float z);
    void SetTheta(float theta);
    void SetPhi(float phi);
    void SetViewVolumeWidth(float width);
    void UpdateViewCoord();
    void InitDragRotation(int x, int y);
    void InitDragTranslation(int x, int y);
    void DragRotation(int x, int y);
    void DragTranslation(int x, int y);
    void WheelZoom(int direction);
    void Moving(Camera::Translate direction);
    void Turning(Camera::Rotate direction);
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
