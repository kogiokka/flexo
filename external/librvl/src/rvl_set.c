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
rvl_set_voxel_dims (RVL *self, float dx, float dy, float dz)
{
  RVLSize size = 1 * sizeof (f32);

  self->grid.dimBufSz = 3 * size;
  self->grid.dimBuf = (RVLByte *)malloc (self->grid.dimBufSz);

  self->grid.ndx = 1;
  self->grid.ndy = 1;
  self->grid.ndz = 1;
  self->grid.dx  = (f32 *)(self->grid.dimBuf);
  self->grid.dy  = (f32 *)(self->grid.dimBuf + size);
  self->grid.dz  = (f32 *)(self->grid.dimBuf + (2 * size));

  ((f32 *)self->grid.dx)[0] = dx;
  ((f32 *)self->grid.dy)[0] = dy;
  ((f32 *)self->grid.dz)[0] = dz;
}

void
rvl_set_voxel_dims_v (RVL *self, int ndx, int ndy, int ndz, float *dx,
                      float *dy, float *dz)
{
  RVLSize sizef32 = sizeof (f32);
  RVLSize szdx    = ndx * sizeof (f32);
  RVLSize szdy    = ndy * sizeof (f32);
  RVLSize szdz    = ndz * sizeof (f32);

  self->grid.dimBufSz = (ndx + ndy + ndz) * sizef32;
  self->grid.dimBuf = (RVLByte *)malloc (self->grid.dimBufSz);

  self->grid.ndx = ndx;
  self->grid.ndy = ndy;
  self->grid.ndz = ndz;
  self->grid.dx  = (f32 *)(self->grid.dimBuf);
  self->grid.dy  = (f32 *)(self->grid.dimBuf + szdx);
  self->grid.dz  = (f32 *)(self->grid.dimBuf + szdx + szdy);

  memcpy (self->grid.dx, dx, szdx);
  memcpy (self->grid.dy, dy, szdy);
  memcpy (self->grid.dz, dz, szdz);
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
