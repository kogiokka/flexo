#include "rvl.h"

#include "detail/rvl_p.h"

RVLGridUnit
rvl_get_grid_unit (RVL *self)
{
  return self->gridUnit;
}

RVLGridType
rvl_get_grid_type (RVL *self)
{
  return self->gridType;
}

RVLPrimitive
rvl_get_primitive (RVL *self)
{
  return self->primitive;
}

RVLEndian
rvl_get_endian (RVL *self)
{
  return self->endian;
}

void
rvl_get_resolution (RVL *self, int *x, int *y, int *z)
{
  *x = self->resolution[0];
  *y = self->resolution[1];
  *z = self->resolution[2];
}

void
rvl_get_voxel_size (RVL *self, float *x, float *y, float *z)
{
  *x = self->voxelSize[0];
  *y = self->voxelSize[1];
  *z = self->voxelSize[2];
}

void
rvl_get_position (RVL *self, float *x, float *y, float *z)
{
  *x = self->position[0];
  *y = self->position[1];
  *z = self->position[2];
}

void
rvl_get_data_buffer (RVL *self, RVLByte **buffer, RVLSize *size)
{
  *buffer = self->data.rbuf;
  *size   = self->data.size;
}

void
rvl_get_text (RVL *self, RVLText **text, int *numText)
{
  *text    = self->text;
  *numText = self->numText;
}

RVLSize
rvl_get_primitive_byte_count (RVL *self)
{
  switch (self->primitive)
    {
    case RVLPrimitive_f8:
    case RVLPrimitive_u8:
    case RVLPrimitive_i8:
      return 1;
    case RVLPrimitive_u16:
    case RVLPrimitive_i16:
    case RVLPrimitive_f16:
      return 2;
    case RVLPrimitive_u32:
    case RVLPrimitive_i32:
    case RVLPrimitive_f32:
      return 4;
    case RVLPrimitive_u64:
    case RVLPrimitive_i64:
    case RVLPrimitive_f64:
      return 8;
    case RVLPrimitive_u128:
    case RVLPrimitive_i128:
    case RVLPrimitive_f128:
      return 16;
      // ----------------------- Vec2
    case RVLPrimitive_vec2f8:
    case RVLPrimitive_vec2u8:
    case RVLPrimitive_vec2i8:
      return 1 * 2;
    case RVLPrimitive_vec2u16:
    case RVLPrimitive_vec2i16:
    case RVLPrimitive_vec2f16:
      return 2 * 2;
    case RVLPrimitive_vec2u32:
    case RVLPrimitive_vec2i32:
    case RVLPrimitive_vec2f32:
      return 4 * 2;
    case RVLPrimitive_vec2u64:
    case RVLPrimitive_vec2i64:
    case RVLPrimitive_vec2f64:
      return 8 * 2;
    case RVLPrimitive_vec2u128:
    case RVLPrimitive_vec2i128:
    case RVLPrimitive_vec2f128:
      return 16 * 2;
      // ----------------------- Vec3
    case RVLPrimitive_vec3u8:
    case RVLPrimitive_vec3i8:
    case RVLPrimitive_vec3f8:
      return 1 * 3;
    case RVLPrimitive_vec3u16:
    case RVLPrimitive_vec3i16:
    case RVLPrimitive_vec3f16:
      return 2 * 3;
    case RVLPrimitive_vec3u32:
    case RVLPrimitive_vec3i32:
    case RVLPrimitive_vec3f32:
      return 4 * 3;
    case RVLPrimitive_vec3u64:
    case RVLPrimitive_vec3i64:
    case RVLPrimitive_vec3f64:
      return 8 * 3;
    case RVLPrimitive_vec3u128:
    case RVLPrimitive_vec3i128:
    case RVLPrimitive_vec3f128:
      return 16 * 3;
      // ----------------------- Vec4
    case RVLPrimitive_vec4u8:
    case RVLPrimitive_vec4i8:
    case RVLPrimitive_vec4f8:
      return 1 * 4;
    case RVLPrimitive_vec4u16:
    case RVLPrimitive_vec4i16:
    case RVLPrimitive_vec4f16:
      return 2 * 4;
    case RVLPrimitive_vec4u32:
    case RVLPrimitive_vec4i32:
    case RVLPrimitive_vec4f32:
      return 4 * 4;
    case RVLPrimitive_vec4u64:
    case RVLPrimitive_vec4i64:
    case RVLPrimitive_vec4f64:
      return 8 * 4;
    case RVLPrimitive_vec4u128:
    case RVLPrimitive_vec4i128:
    case RVLPrimitive_vec4f128:
      return 16 * 4;
    default:
      return 0;
    }
}
