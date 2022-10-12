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
  return self->gridUnit;
}

RVLValueDimen
rvl_get_value_dimension (RVL *self)
{
  return self->valueDimen;
}

RVLValueFormat
rvl_get_value_format (RVL *self)
{
  return self->valueFormat;
}

RVLValueBitDepth
rvl_get_value_bit_depth (RVL *self)
{
  return self->valueBitDepth;
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
  *size = self->data.size;
}

void
rvl_get_text (RVL *self, RVLText **text, int *numText)
{
  *text = self->text;
  *numText = self->numText;
}

RVLSize
rvl_get_value_byte_count (RVL *self)
{
  RVLSize count = 1;

  switch (self->valueBitDepth)
    {
    case RVLValueBitDepth_8:
      count *= 1;
      break;
    case RVLValueBitDepth_16:
      count *= 2;
      break;
    case RVLValueBitDepth_32:
      count *= 3;
      break;
    case RVLValueBitDepth_64:
      count *= 4;
      break;
    case RVLValueBitDepth_128:
      count *= 5;
      break;
    }

  switch (self->valueDimen)
    {
    case RVLValueDimen_Scalar:
      count *= 1;
      break;
    case RVLValueDimen_Vec2:
      count *= 2;
      break;
    case RVLValueDimen_Vec3:
      count *= 3;
      break;
    case RVLValueDimen_Vec4:
      count *= 4;
      break;
    }

  return count;
}
