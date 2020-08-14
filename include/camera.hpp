#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>
#include <tuple>

class Camera
{
  glm::vec3 m_position;
  glm::vec3 m_worldUp;
  glm::vec3 m_forward;
  glm::vec3 m_side;
  glm::vec3 m_up;
  glm::vec3 m_center;
  float m_theta;
  float m_phi;
  float m_radius;
  float m_aspectRatio;
  float m_viewVolumeSize;
  float m_moveRate;
  float m_rotateRate;
  float m_zoom;
  int m_horizontalCoef;

  std::tuple<int, int, float, float> m_originRotation;
  std::tuple<float, float, glm::vec3> m_originTranslation;

  Camera(Camera const&) = delete;
  Camera& operator=(Camera const&) = delete;

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
  Camera(int width, int height, Camera::Projection projection = Camera::Projection::Orthographic);
  ~Camera();
  void SetAspectRatio(float ratio);
  void SetAspectRatio(int width, int height);
  void SetCenter(float x, float y, float z);
  void SetTheta(float theta);
  void SetPhi(float phi);
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
  Projection m_projection;
  glm::vec3 CartesianCoord(float theta, float phi) const;
  inline float NormRadian(float radian);
};
