#include <stdlib.h>
#include <string.h>

#include <log.h>
#include <lz4.h>
#include <lz4hc.h>

#include "rvl.h"

#include "detail/rvl_p.h"
#include "detail/rvl_text_p.h"

static void rvl_write_chunk_header (RVL *self, RVLChunkCode code,
                                    RVLSize size);
static void rvl_write_chunk_payload (RVL *self, RVLConstByte *data,
                                     RVLSize size);
static void rvl_write_chunk_end (RVL *self);

static void rvl_write_VHDR_chunk (RVL *self);
static void rvl_write_GRID_chunk (RVL *self);
static void rvl_write_DATA_chunk (RVL *self);
static void rvl_write_TEXT_chunk (RVL *self, const RVLText *textArr,
                                  int numText);
static void rvl_write_VEND_chunk (RVL *self);

static void rvl_write_file_sig (RVL *self);
static void rvl_fwrite (RVL *self, RVLConstByte *data, RVLSize size);

static void check_grid (RVL *self);
static void check_data (RVL *self);

void
rvl_write_rvl (RVL *self)
{
  check_data (self);
  check_grid (self);

  rvl_write_file_sig (self);

  // Required chunks
  rvl_write_VHDR_chunk (self);
  rvl_write_GRID_chunk (self);
  rvl_write_DATA_chunk (self);

  if (self->text != NULL)
    {
      rvl_write_TEXT_chunk (self, self->text, self->numText);
    }

  rvl_write_VEND_chunk (self);
}

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
  RVLSize  offset   = 14;
  RVLSize  wbufSize = offset + self->grid.dimBufSz;
  RVLByte *wbuf     = (RVLByte *)malloc (wbufSize);

  // Grid info
  wbuf[0] = self->grid.type;
  wbuf[1] = self->grid.unit;
  memcpy (&wbuf[2], self->grid.position, 12);

  RVLSize szdx = self->grid.ndx * sizeof (f32);
  RVLSize szdy = self->grid.ndy * sizeof (f32);
  RVLSize szdz = self->grid.ndz * sizeof (f32);

  memcpy (&wbuf[offset], self->grid.dx, szdx);
  offset += szdx;
  memcpy (&wbuf[offset], self->grid.dy, szdy);
  offset += szdy;
  memcpy (&wbuf[offset], self->grid.dz, szdz);

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

  rvl_fwrite (self, buf, 8);
}

void
rvl_write_chunk_payload (RVL *self, RVLConstByte *data, RVLSize size)
{
  if (data != NULL && size > 0)
    {
      rvl_fwrite (self, data, size);
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
  rvl_fwrite (self, RVL_FILE_SIG, RVL_FILE_SIG_SIZE);
}

void
rvl_fwrite (RVL *self, RVLConstByte *data, RVLSize size)
{
  if (self->writeFn == NULL)
    {
      log_fatal ("[librvl write] Call to NULL write function. Please check if "
                 "the RVL instance is a writer.");
      exit (EXIT_FAILURE);
    }

  self->writeFn (self, data, size);
}

void
rvl_fwrite_default (RVL *self, RVLConstByte *data, RVLSize size)
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

void
check_data (RVL *self)
{
  if (self->data.size <= 0)
    {
      log_fatal ("[librvl write] Data buffer size is less than 0.");
      exit (EXIT_FAILURE);
    }

  if (self->data.size != rvl_get_data_nbytes (self))
    {
      log_fatal ("[librvl write] Data buffer size does not match this RVL "
                 "configuration.");
      exit (EXIT_FAILURE);
    }
}

void
check_grid (RVL *self)
{
  if (self->grid.ndx <= 0 || self->grid.ndy <= 0 || self->grid.ndz <= 0)
    {
      log_fatal ("[librvl write] Missing voxel dimensions.");
      exit (EXIT_FAILURE);
    }

  switch (self->grid.type)
    {
    case RVLGridType_Regular:
      if (self->grid.ndx != 1 || self->grid.ndy != 1 || self->grid.ndz != 1)
        {
          log_fatal ("[librvl write] Number of voxel dimensions is not valid "
                     "for regular grid.");
          exit (EXIT_FAILURE);
        }
      break;
    case RVLGridType_Rectilinear:
      {
        u32 *r = self->resolution;
        if (self->grid.ndx != r[0] || self->grid.ndy != r[1]
            || self->grid.ndz != r[2])
          {
            log_fatal (
                "[librvl write] Number of voxel dimensions do not match "
                "the resolution.");
            exit (EXIT_FAILURE);
          }
      }
      break;
    default:
      log_fatal ("[librvl write] Invalid grid type: %.2x.", self->grid.type);
      exit (EXIT_FAILURE);
      break;
    }
}
