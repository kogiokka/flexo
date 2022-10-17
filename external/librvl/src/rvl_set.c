#include <stdlib.h>
#include <string.h>

#include "rvl.h"

#include "detail/rvl_p.h"

void
rvl_set_grid_type (RVL *self, RVLGridType gridType)
{
  self->grid.type = gridType;
}

void
rvl_set_grid_unit (RVL *self, RVLGridUnit gridUnit)
{
  self->grid.unit = gridUnit;
}

void
rvl_set_primitive (RVL *self, RVLPrimitive primitive)
{
  self->primitive = primitive;
}

void
rvl_set_endian (RVL *self, RVLEndian endian)
{
  self->endian = endian;
}

void
rvl_set_resolution (RVL *self, int x, int y, int z)
{
  self->resolution[0] = x;
  self->resolution[1] = y;
  self->resolution[2] = z;
}

void
rvl_set_grid_position (RVL *self, float x, float y, float z)
{
  self->grid.position[0] = x;
  self->grid.position[1] = y;
  self->grid.position[2] = z;
}

void
rvl_set_voxel_dims_1f (RVL *self, float x)
{
  RVLSize size = 1 * sizeof (f32);

  rvl_alloc (self, &self->grid.vxDimBuf, size);
  memcpy (self->grid.vxDimBuf, &x, size);
  self->grid.vxDimBufSize = size;
}

void
rvl_set_voxel_dims_3f (RVL *self, float x, float y, float z)
{
  RVLSize size = 3 * sizeof (f32);

  rvl_alloc (self, &self->grid.vxDimBuf, size);
  f32 arr[3] = { x, y, z };
  memcpy (self->grid.vxDimBuf, arr, size);
  self->grid.vxDimBufSize = size;
}

void
rvl_set_voxel_dims_v (RVL *self, int n, const float *dimensions)
{
  RVLSize size = n * sizeof (f32);

  rvl_alloc (self, &self->grid.vxDimBuf, size);
  memcpy (self->grid.vxDimBuf, dimensions, size);
  self->grid.vxDimBufSize = size;
}

void
rvl_set_data_buffer (RVL *self, RVLSize size, RVLConstByte *buffer)
{
  self->data.size = size;
  self->data.wbuf = buffer;
}

void
rvl_set_text (RVL *self, RVLText **text, int numText)
{
  rvl_text_destroy_array (&self->text);

  self->text    = *text;
  self->numText = numText;
  *text         = NULL;
}
