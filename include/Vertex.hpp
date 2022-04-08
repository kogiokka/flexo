#pragma once
#include <type_traits>

/* Standard Layout Type
 * https://en.cppreference.com/w/cpp/named_req/StandardLayoutType
 *
 * Make sure the elements are continuous in the memory.
 */
struct Vertex {
  struct Position {
    float x, y, z;
  };

  struct Normal {
    float x, y, z;
  };

  Position position;
  Normal normal;
};

static_assert(std::is_standard_layout_v<Vertex>);
static_assert(std::is_standard_layout_v<Vertex::Position>);
static_assert(std::is_standard_layout_v<Vertex::Normal>);
