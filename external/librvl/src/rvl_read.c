#include <stdio.h>
#include <stdlib.h>

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
#ifndef NDEBUG
      char *ch = (char *)&code;
      printf ("# Reading header\n"
              "    size: %d\n"
              "    code: %c%c%c%c\n",
              size, ch[0], ch[1], ch[2], ch[3]);
#endif

      switch (code)
        {
        case RVLChunkCode_VHDR:
          rvl_read_VHDR_chunk (self, size);
          break;
        case RVLChunkCode_DATA:
          rvl_alloc_data_buffer (self, &self->data.rbuf, &self->data.size);
          rvl_read_DATA_chunk (self, size);
          break;
        case RVLChunkCode_TEXT:
          rvl_read_TEXT_chunk (self, size);
          break;
        case RVLChunkCode_VEND:
          break;
        default:
          {
            RVLConstByte *ch = (RVLConstByte *)&code;
            fprintf (stderr, "Unknown chunk code: %c%c%c%c", ch[0], ch[1],
                     ch[2], ch[3]);
          }
          break;
        }
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

      switch (code)
        {
        case RVLChunkCode_VHDR:
          rvl_read_VHDR_chunk (self, size);
          break;
        case RVLChunkCode_TEXT:
          rvl_read_TEXT_chunk (self, size);
          break;
        default:
          fseek (self->io, size, SEEK_CUR);
          break;
        }
    }
  while (code != RVLChunkCode_VEND);
  fseek (self->io, RVL_FILE_SIG_SIZE, SEEK_SET);
}

void
rvl_read_data_buffer (RVL *self, RVLByte **data)
{
  if (self == NULL)
    return;

  self->data.rbuf = *data;

  RVLChunkCode code;
  do
    {
      RVLSize size;
      rvl_read_chunk_header (self, &size, &code);

      if (code == RVLChunkCode_DATA)
        {
          rvl_read_DATA_chunk (self, size);
          break;
        }
      else
        {
          fseek (self->io, size, SEEK_CUR);
        }
    }
  while (code != RVLChunkCode_VEND);

  self->data.rbuf = NULL;
  fseek (self->io, RVL_FILE_SIG_SIZE, SEEK_SET);
}
