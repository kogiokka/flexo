#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <log.h>
#include <lz4.h>
#include <lz4hc.h>

#include "detail/rvl_write_p.h"

#include "detail/rvl_text_p.h"

void
rvl_write_VHDR_chunk (RVL *self)
{
  RVLSize size = 18;
  u8     *buf  = calloc (1, size);

  memcpy (&buf[0], self->version, 2);
  memcpy (&buf[2], &self->resolution[0], 12);
  memcpy (&buf[14], &self->primitive, 2);
  buf[16] = self->endian;
  buf[17] = 0; // padding

  rvl_write_chunk_header (self, RVLChunkCode_VHDR, size);
  rvl_write_chunk_payload (self, buf, size);
  rvl_write_chunk_end (self);
}

void
rvl_write_GRID_chunk (RVL *self)
{
  RVLSize  wbufSize = 14 + self->grid.vxDimBufSize;
  RVLByte *wbuf     = (RVLByte *)malloc (wbufSize);

  // Grid info
  wbuf[0] = self->grid.type;
  wbuf[1] = self->grid.unit;
  memcpy (&wbuf[2], self->grid.position, 12);
  memcpy (&wbuf[14], self->grid.vxDimBuf, self->grid.vxDimBufSize);

  rvl_write_chunk_header (self, RVLChunkCode_GRID, wbufSize);
  rvl_write_chunk_payload (self, wbuf, wbufSize);
  rvl_write_chunk_end (self);
}

void
rvl_write_DATA_chunk (RVL *self)
{
  RVLSize wbufSize = self->data.size;
  char   *wbuf     = (char *)malloc (wbufSize);

  int         srcSize = self->data.size;
  const char *src     = (char *)self->data.wbuf;

  int compSize
      = LZ4_compress_HC (src, wbuf, srcSize, wbufSize, LZ4HC_CLEVEL_MIN);

  // When the compressed size is greater than the uncompressed one.
  if (compSize == 0)
    {
      wbufSize = LZ4_compressBound (self->data.size);
      wbuf     = realloc (wbuf, wbufSize);
      compSize = LZ4_compress_HC (src, wbuf, self->data.size, wbufSize,
                                  LZ4HC_CLEVEL_MIN);
    }

  rvl_write_chunk_header (self, RVLChunkCode_DATA, compSize);
  rvl_write_chunk_payload (self, (RVLConstByte *)wbuf, compSize);
  rvl_write_chunk_end (self);

  free (wbuf);
}

// Strip off the null terminator at the end of the value string.
void
rvl_write_TEXT_chunk (RVL *self, const RVLText *textArr, int numText)
{
  for (int i = 0; i < numText; i++)
    {
      const RVLText *const text      = &textArr[i];
      const RVLSize        keySize   = strlen (text->key);
      const RVLSize        valueSize = strlen (text->value);

      rvl_write_chunk_header (self, RVLChunkCode_TEXT,
                              keySize + valueSize + 1);

      // Include the null terminator
      rvl_write_chunk_payload (self, (RVLConstByte *)text->key, keySize + 1);

      if (valueSize != 0)
        {
          rvl_write_chunk_payload (self, (RVLConstByte *)text->value,
                                   valueSize);
        }

      rvl_write_chunk_end (self);
    }
}

void
rvl_write_VEND_chunk (RVL *self)
{
  rvl_write_chunk_header (self, RVLChunkCode_VEND, 0);
  rvl_write_chunk_payload (self, NULL, 0);
  rvl_write_chunk_end (self);
}

void
rvl_write_chunk_header (RVL *self, RVLChunkCode code, RVLSize size)
{
  u8 buf[8];

  // Chunk Size(bytes)
  memcpy (buf, &size, 4);
  // Chunk Code
  memcpy (&buf[4], &code, 4);

  rvl_write_data (self, buf, 8);
}

void
rvl_write_chunk_payload (RVL *self, RVLConstByte *data, RVLSize size)
{
  if (data != NULL && size > 0)
    {
      rvl_write_data (self, data, size);
    }
}

void
rvl_write_chunk_end (RVL *self)
{
  return;
}

void
rvl_write_file_sig (RVL *self)
{
  rvl_write_data (self, RVL_FILE_SIG, RVL_FILE_SIG_SIZE);
}

void
rvl_write_data (RVL *self, RVLConstByte *data, RVLSize size)
{
  if (self->writeData == NULL)
    {
      log_fatal ("[librvl write] The write function is NULL. Please check if "
                 "the RVL instance is a writer.");
      exit (EXIT_FAILURE);
    }

  self->writeData (self, data, size);
}

void
rvl_write_data_default (RVL *self, RVLConstByte *data, RVLSize size)
{
  if (self->io == NULL)
    {
      log_fatal ("[librvl write] RVL is not initialized.");
      exit (EXIT_FAILURE);
    }

  const size_t count = fwrite (data, 1, size, self->io);

  if (count != size)
    {
      log_fatal ("[librvl write] fwrite failure.");
      exit (EXIT_FAILURE);
    }
}
