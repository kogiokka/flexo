#include "Camera.hpp"

Camera::Camera(int width, int height, Camera::Projection projection)
  : m_worldUp{0.0f, 1.0f, 0.0f}
  , m_center{0.0f, 0.0f, 0.0f}
  , m_theta(0.0f)
  , m_phi(1.5707f)
  , m_radius(100.f)
  , m_viewVolumeSize(100.f)
  , m_moveRate(0.4f)
  , m_rotateRate(0.005f)
  , m_zoom(0.8f)
  , m_horizontalCoef(1)
  , m_projection(Projection::Perspective)
{
  m_projection = projection;
  SetAspectRatio(width, height);
  UpdateViewCoord();
}

Camera::~Camera() {}

glm::vec3
Camera::CartesianCoord(float theta, float phi) const
{
  using namespace std;

  float const sin_p = sinf(phi);
  float const cos_p = cosf(phi);
  float const sin_t = sinf(theta);
  float const cos_t = cosf(theta);

  return glm::vec3{sin_p * cos_t, cos_p, sin_p * sin_t};
}

void
Camera::UpdateViewCoord()
{
  using namespace glm;

  constexpr float degree_45 = 0.785f;
  m_worldUp = normalize(-m_radius * CartesianCoord(m_theta, m_phi + degree_45));

  m_position = m_radius * CartesianCoord(m_theta, m_phi) + m_center;
  m_forward = normalize(m_center - m_position);
  m_side = normalize(cross(m_forward, m_worldUp));
  m_up = cross(m_side, m_forward);
}

void
Camera::InitDragRotation(int x, int y)
{
  constexpr float half_round = 3.1415926f;

  // Change rotating direction if the camera passes through the polars.
  // Phi is guaranteed to be on [0, pi] or (pi, 2 * pi).
  if (m_phi > half_round)
    m_horizontalCoef = -1;
  else
    m_horizontalCoef = 1;

  m_originRotation = std::make_tuple(x, y, m_theta, m_phi);
}

void
Camera::DragRotation(int x, int y)
{
  auto const [x_o, y_o, theta_o, phi_o] = m_originRotation;

  m_theta = NormRadian(m_horizontalCoef * (x - x_o) * m_rotateRate + theta_o);
  m_phi = NormRadian(-(y - y_o) * m_rotateRate + phi_o);

  UpdateViewCoord();
}

void
Camera::InitDragTranslation(int x, int y)
{
  m_originTranslation = std::make_tuple(x, y, m_center);
}

void
Camera::DragTranslation(int x, int y)
{
  auto const [x_o, y_o, target_o] = m_originTranslation;

  m_center = target_o + (-(x - x_o) * m_side + (y - y_o) * m_up) * m_moveRate * m_zoom;

  UpdateViewCoord();
}

void
Camera::SetAspectRatio(float ratio)
{
  m_aspectRatio = ratio;
}

void
Camera::SetAspectRatio(int width, int height)
{
  m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

void
Camera::SetCenter(float x, float y, float z)
{
  m_center = glm::vec3{x, y, z};
  UpdateViewCoord();
}

void
Camera::SetPhi(float phi)
{
  m_phi = NormRadian(phi);
  UpdateViewCoord();
}

void
Camera::SetTheta(float theta)
{
  m_theta = NormRadian(theta);
  UpdateViewCoord();
}

void
Camera::WheelZoom(int direction)
{
  switch (m_projection) {
  case Projection::Orthographic: {
    float const tmp_zoom = m_zoom + direction * 0.02;
    constexpr float min = 0.01f;
    constexpr float max = 1.0f;

    if (tmp_zoom < min) {
      m_zoom = min;
    } else if (tmp_zoom >= max) {
      m_zoom = max;
    } else {
      m_zoom = tmp_zoom;
    }
  } break;
  case Projection::Perspective: {
    m_radius += direction * 2.0f;
    UpdateViewCoord();
  } break;
  }
}

glm::mat4
Camera::ViewMatrix() const
{
  return glm::lookAt(m_position, m_center, m_worldUp);
}

glm::mat4
Camera::ProjectionMatrix() const
{
  assert(m_aspectRatio != 0.0);
  glm::mat4 mat;
  switch (m_projection) {
  case Projection::Orthographic: {
    float const v = m_viewVolumeSize * m_aspectRatio * m_zoom;
    float const h = m_viewVolumeSize * m_zoom;
    mat = glm::ortho(-v, v, -h, h, 0.1f, 1000.f);
  } break;
  case Projection::Perspective: {
    mat = glm::perspective(glm::radians(45.0f), m_aspectRatio, 0.1f, 2000.0f);
  } break;
  }

  return mat;
}

glm::mat4
Camera::ViewProjectionMatrix() const
{
  return ProjectionMatrix() * ViewMatrix();
}

void
Camera::SetProjection(Camera::Projection projection)
{
  m_projection = projection;
}

glm::vec3 const
Camera::Position() const
{
  return m_position;
}

glm::vec3 const
Camera::ForwardVector() const
{
  return m_forward;
}

void
Camera::Moving(Camera::Translate direction)
{
  float const speed = 10.f * m_moveRate * m_zoom;
  switch (direction) {
  case Translate::Up:
    m_center += -m_up * speed;
    break;
  case Translate::Down:
    m_center += m_up * speed;
    break;
  case Translate::Right:
    m_center += -m_side * speed;
    break;
  case Translate::Left:
    m_center += m_side * speed;
    break;
  case Translate::Forward:
    m_center += m_forward * speed;
    break;
  case Translate::Backward:
    m_center += -m_forward * speed;
    break;
  }

  UpdateViewCoord();
}

void
Camera::Turning(Camera::Rotate direction)
{
  switch (direction) {
  case Rotate::Clockwise:
    m_theta = m_theta + 0.1f;
    break;
  case Rotate::CounterClockwise:
    m_theta = m_theta - 0.1f;
    break;
  case Rotate::PitchUp:
    m_phi = m_phi - 0.1f;
    break;
  case Rotate::PitchDown:
    m_phi = m_phi + 0.1f;
    break;
  }

  m_phi = NormRadian(m_phi);
  m_theta = NormRadian(m_theta);
  UpdateViewCoord();
}

// Restrict both theta and phi to being between 0 and 360 degrees.
float
Camera::NormRadian(float radian)
{
  constexpr float full_round = 6.2831853f;

  if (radian < 0.f)
    return radian + full_round;
  else if (radian > full_round)
    return radian - full_round;
  else
    return radian;
}
