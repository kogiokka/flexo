#include <stdio.h>
#include <stdlib.h>

#include <log.h>

#include "rvl.h"

#include "detail/rvl_read_p.h"

void
rvl_read_rvl (RVL *self)
{
  if (self == NULL)
    return;

  rvl_read_file_sig (self);

  RVLChunkCode code;
  do
    {
      RVLSize size;
      rvl_read_chunk_header (self, &size, &code);

      char *ch = (char *)&code;
      log_debug ("[librvl read] Reading chunk header: size %d, code: %c%c%c%c",
                 size, ch[0], ch[1], ch[2], ch[3]);

      RVLByte *rbuf = (RVLByte *)malloc (size);
      switch (code)
        {
        case RVLChunkCode_VHDR:
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_VHDR_chunk (self, rbuf, size);
          break;
        case RVLChunkCode_GRID:
          rvl_alloc (self, &self->grid.vxDimBuf, (size - 14));
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_GRID_chunk (self, rbuf, size);
          break;
        case RVLChunkCode_DATA:
          rvl_alloc (self, &self->data.rbuf, self->data.size);
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_DATA_chunk (self, rbuf, size);
          break;
        case RVLChunkCode_TEXT:
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_TEXT_chunk (self, rbuf, size);
          break;
        case RVLChunkCode_VEND:
          break;
        default:
          {
            log_warn ("[librvl read] Unknown chunk code: %c%c%c%c", ch[0],
                      ch[1], ch[2], ch[3]);
          }
          break;
        }
      free (rbuf);
    }
  while (code != RVLChunkCode_VEND);
}

void
rvl_read_info (RVL *self)
{
  if (self == NULL)
    return;

  rvl_read_file_sig (self);

  RVLChunkCode code;
  do
    {
      RVLSize size;
      rvl_read_chunk_header (self, &size, &code);

      RVLByte *rbuf = (RVLByte *)malloc (size);
      switch (code)
        {
        case RVLChunkCode_VHDR:
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_VHDR_chunk (self, rbuf, size);
          break;
        case RVLChunkCode_GRID:
          rvl_alloc (self, &self->grid.vxDimBuf, (size - 14));
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_GRID_chunk (self, rbuf, size);
          break;
        case RVLChunkCode_TEXT:
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_TEXT_chunk (self, rbuf, size);
          break;
        default:
          fseek (self->io, size, SEEK_CUR);
          break;
        }
      free (rbuf);
    }
  while (code != RVLChunkCode_VEND);
  fseek (self->io, RVL_FILE_SIG_SIZE, SEEK_SET);
}

void
rvl_read_data_buffer (RVL *self, RVLByte **buffer)
{
  if (self == NULL)
    return;

  self->data.rbuf = *buffer;

  RVLChunkCode code;
  do
    {
      RVLSize size;
      rvl_read_chunk_header (self, &size, &code);

      RVLByte *rbuf = (RVLByte *)malloc (size);

      if (code == RVLChunkCode_DATA)
        {
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_DATA_chunk (self, rbuf, size);
          break;
        }
      else
        {
          fseek (self->io, size, SEEK_CUR);
        }

      free (rbuf);
    }
  while (code != RVLChunkCode_VEND);

  self->data.rbuf = NULL;
  fseek (self->io, RVL_FILE_SIG_SIZE, SEEK_SET);
}
