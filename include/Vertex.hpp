#pragma once

#include <glm/glm.hpp>
#include <type_traits>

struct Vertex {
  using Position = glm::vec3;
  using Normal = glm::vec3;

  Position position;
  Normal normal;
};

static_assert(sizeof(Vertex) == 24);
