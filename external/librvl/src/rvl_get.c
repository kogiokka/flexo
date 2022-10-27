#include <stdlib.h>

#include <log.h>

#include "rvl.h"

#include "detail/rvl_p.h"
#include "detail/rvl_text_p.h"

void
rvl_get_volumetric_format (RVL *self, int *nx, int *ny, int *nz,
                           RVLenum *primitive, RVLenum *endian)
{
  *nx        = self->resolution[0];
  *ny        = self->resolution[1];
  *nz        = self->resolution[2];
  *primitive = self->primitive;
  *endian    = self->endian;
}

RVLenum
rvl_get_compression (RVL *self)
{
  return self->compress;
}

RVLenum
rvl_get_grid_unit (RVL *self)
{
  return self->grid.unit;
}

RVLenum
rvl_get_grid_type (RVL *self)
{
  return self->grid.type;
}

void
rvl_get_voxel_dims (RVL *self, float *dx, float *dy, float *dz)
{
  *dx = ((float *)self->grid.dx)[0];
  *dy = ((float *)self->grid.dy)[0];
  *dz = ((float *)self->grid.dz)[0];
}

void
rvl_get_voxel_dims_v (RVL *self, int *ndx, int *ndy, int *ndz,
                      const float **dx, const float **dy, const float **dz)
{
  *ndx = self->grid.ndx;
  *ndy = self->grid.ndy;
  *ndz = self->grid.ndz;
  *dx  = (const float *)self->grid.dx;
  *dy  = (const float *)self->grid.dy;
  *dz  = (const float *)self->grid.dz;
}

void
rvl_get_grid_position (RVL *self, float *x, float *y, float *z)
{
  *x = self->grid.position[0];
  *y = self->grid.position[1];
  *z = self->grid.position[2];
}

void
rvl_get_data_buffer (RVL *self, const void **buffer)
{
  *buffer = (unsigned char *)self->data.rbuf;
}

void
rvl_get_text (RVL *self, RVLenum tag, const char **value)
{
  RVLText *cur = self->text;
  while (cur != NULL)
    {
      if (cur->tag == tag)
        {
          log_trace ("[librvl get] Get TEXT: %.4X, %s", cur->tag, cur->value);
          *value = cur->value;
          return;
        }
      cur = cur->next;
    }

  *value = "";
}

unsigned int
rvl_get_primitive_nbytes (RVL *self)
{
  BYTE *p = (BYTE *)&self->primitive;

  u8 dimen = p[1];
  u8 bytes = (1 << (p[0] & 0x0f)) / 8;

  u32 nbytes = dimen * bytes;

  if (nbytes <= 0)
    {
      log_error ("[librvl get] Invalid primitive: %.4x", self->primitive);
    }

  return nbytes;
}

unsigned int
rvl_get_data_nbytes (RVL *self)
{
  const u32 *res = self->resolution;

  if (res[0] <= 0 || res[1] <= 0 || res[2] <= 0)
    {
      log_error ("[librvl get] Invalid resolution: %d, %d, %d", res[0], res[1],
                 res[2]);
    }

  return res[0] * res[1] * res[2] * rvl_get_primitive_nbytes (self);
}
