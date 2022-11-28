#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "rvl.h"

#include "detail/rvl_log_p.h"
#include "detail/rvl_p.h"
#include "detail/rvl_text_p.h"

static void rvl_set_voxel_dims (RVL *self, float dx, float dy, float dz);
static void rvl_set_voxel_dims_v (RVL *self, int ndx, int ndy, int ndz,
                                  float *dx, float *dy, float *dz);
void
rvl_set_file (RVL *self, const char *filename)
{

  if (self->isOwningIo && self->io != NULL)
    {
      rvl_log_debug ("Closing old file stream...");
      fclose (self->io);
    }

  switch (self->ioState)
    {
    case RVLIoState_Read:
      self->io = fopen (filename, "rb");
      break;
    case RVLIoState_Write:
      self->io = fopen (filename, "wb");
      break;
    }

  if (self->io == NULL)
    {
      rvl_log_error ("fopen failed: %s", strerror (errno));
    }

  rvl_log_debug ("File stream has been set to file \"%s\".", filename);
  self->isOwningIo = true;
}

void
rvl_set_io (RVL *self, FILE *stream)
{
  if (self->isOwningIo && self->io != NULL)
    {
      rvl_log_debug ("Closing old file stream...");
      fclose (self->io);
    }

  self->io = stream;

  rvl_log_debug ("File stream has been set to %p", stream);
  self->isOwningIo = false;
}

void
rvl_set_volumetric_format (RVL *self, int nx, int ny, int nz,
                           RVLenum primitive, RVLenum endian)
{
  self->resolution[0] = nx;
  self->resolution[1] = ny;
  self->resolution[2] = nz;
  self->primitive     = primitive;
  self->endian        = endian;
}

void
rvl_set_compression (RVL *self, RVLenum compression)
{
  self->compress = compression;
}

void
rvl_set_regular_grid (RVL *self, float dx, float dy, float dz)
{
  self->grid.type = RVL_GRID_REGULAR;
  rvl_set_voxel_dims (self, dx, dy, dz);
}

void
rvl_set_rectilinear_grid (RVL *self, int ndx, int ndy, int ndz, float *dx,
                          float *dy, float *dz)
{
  self->grid.type = RVL_GRID_RECTILINEAR;
  rvl_set_voxel_dims_v (self, ndx, ndy, ndz, dx, dy, dz);
}

void
rvl_set_grid_unit (RVL *self, RVLenum gridUnit)
{
  self->grid.unit = gridUnit;
}

void
rvl_set_grid_origin (RVL *self, float x0, float y0, float z0)
{
  self->grid.position[0] = x0;
  self->grid.position[1] = y0;
  self->grid.position[2] = z0;
}

void
rvl_set_voxel_dims (RVL *self, float dx, float dy, float dz)
{
  u32 size = 1 * sizeof (f32);

  self->grid.dimBufSz = 3 * size;
  self->grid.dimBuf   = (BYTE *)malloc (self->grid.dimBufSz);

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
  u32 sizef32 = sizeof (f32);
  u32 szdx    = ndx * sizeof (f32);
  u32 szdy    = ndy * sizeof (f32);
  u32 szdz    = ndz * sizeof (f32);

  self->grid.dimBufSz = (ndx + ndy + ndz) * sizef32;
  self->grid.dimBuf   = (BYTE *)malloc (self->grid.dimBufSz);

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
rvl_set_voxels (RVL *self, const void *voxels)
{
  self->data.size = rvl_eval_voxels_nbytes (self);
  self->data.wbuf = (BYTE *)voxels;
}

void
rvl_set_data_buffer (RVL *self, unsigned int size, const void *buffer)
{
  self->data.size = size;
  self->data.wbuf = (BYTE *)buffer;
}

void
rvl_set_text (RVL *self, RVLenum tag, const char *value)
{
  if ((tag & 0xff00) != 0x0D00)
    {
      rvl_log_error ("Invalid enum for TEXT chunk.");
      return;
    }

  if (self->text == NULL)
    {
      RVLText *text = rvl_text_create ();
      rvl_text_set_field (text, tag, value);
      self->text = text;
      return;
    }

  RVLText *cur = self->text;
  while (cur->next != NULL)
    {
      if (cur->tag == tag)
        {
          rvl_log_error ("The text field %.4x has already exist.");
          return;
        }

      cur = cur->next;
    }

  RVLText *text = rvl_text_create ();
  rvl_text_set_field (text, tag, value);
  cur->next = text;
}
