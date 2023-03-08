#include <assert.h>
#include <lzma.h>
#include <stdlib.h>
#include <string.h>

#include "rvl.h"

#include "detail/rvl_log_p.h"
#include "detail/rvl_p.h"
#include "detail/rvl_text_p.h"

// .RVL FORMAT\0
BYTE RVL_FILE_SIG[RVL_FILE_SIG_SIZE] = {
  131, 82, 86, 76, 32, 70, 79, 82, 77, 65, 84, 0,
};

static RVL *rvl_create (RVLIoState ioState);

RVL *
rvl_create_writer (void)
{
  RVL *rvl     = rvl_create (RVLIoState_Write);
  rvl->writeFn = rvl_fwrite_default;
  return rvl;
}

RVL *
rvl_create_reader (void)
{
  RVL *rvl    = rvl_create (RVLIoState_Read);
  rvl->readFn = rvl_fread_default;
  return rvl;
}

void
rvl_destroy (RVL **self)
{
  if (*self == NULL)
    return;

  RVL *ptr = *self;

  // Dealloc read buffer. Writer buffer pointer is non-owning so the user is
  // responsible for calling this dealloc function.
  rvl_dealloc (ptr, &ptr->data.rbuf);
  rvl_dealloc (ptr, &ptr->grid.dimBuf);

  if (ptr->text != NULL)
    {
      RVLText *cur = ptr->text;
      while (cur != NULL)
        {
          RVLText *tmp = cur;
          cur          = cur->next;
          free (tmp->value);
          free (tmp);
        }
    }

  if (ptr->io != NULL)
    {
      fclose (ptr->io);
    }

  free (ptr);

  *self = NULL;
}

void
rvl_alloc (RVL *self, BYTE **ptr, u32 size)
{
  assert (self != NULL || ptr != NULL);

  if (*ptr != NULL)
    {
      free (*ptr);
    }

  *ptr = (BYTE *)calloc (1, size);

  if (*ptr == NULL)
    {
      rvl_log_fatal ("Memory allocation failure.");
      exit (EXIT_FAILURE);
    }
}

void
rvl_dealloc (RVL *self, BYTE **ptr)
{
  assert (self != NULL || ptr != NULL);

  if (*ptr == NULL)
    return;

  free (*ptr);
  *ptr = NULL;
}

RVL *
rvl_create (RVLIoState ioState)
{
  RVL *self        = (RVL *)calloc (1, sizeof (RVL));
  self->version[0] = RVL_VERSION_MAJOR;
  self->version[1] = RVL_VERSION_MINOR;
  self->ioState    = ioState;
  self->text       = NULL;

  rvl_reset_crc32 (self);

  // Explicitly set the default values of the optional settings.
  self->compress         = RVL_COMPRESSION_LZMA2;
  self->grid.unit        = RVL_UNIT_NA;
  self->grid.position[0] = 0.0f;
  self->grid.position[1] = 0.0f;
  self->grid.position[2] = 0.0f;

  return self;
}

void
rvl_calculate_crc32 (RVL *self, const BYTE *buf, u32 size)
{
  self->crc = lzma_crc32 (buf, size, self->crc);
}

void
rvl_reset_crc32 (RVL *self)
{
  self->crc = 0xffffffff;
}

unsigned int
rvl_sizeof (RVLenum primitive)
{
  BYTE *p = (BYTE *)&primitive;

  u8 dimen = p[1];
  u8 bytes = (1 << (p[0] & 0x0f)) / 8;

  u32 nbytes = dimen * bytes;

  if (nbytes <= 0)
    {
      rvl_log_error ("Invalid primitive: %.4x", primitive);
    }

  return nbytes;
}

unsigned int
rvl_eval_voxels_nbytes (RVL *self)
{
  const u32 *res = self->resolution;

  if (res[0] <= 0 || res[1] <= 0 || res[2] <= 0)
    {
      rvl_log_error ("Invalid resolution: %d, %d, %d", res[0], res[1], res[2]);
    }

  return res[0] * res[1] * res[2] * rvl_sizeof (self->primitive);
}

void
rvl_save_u32le (BYTE *buf, u32 num)
{
  buf[0] = (BYTE)num;
  buf[1] = (BYTE)(num >> 8);
  buf[2] = (BYTE)(num >> 16);
  buf[3] = (BYTE)(num >> 24);
}
