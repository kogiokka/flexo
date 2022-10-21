#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <log.h>
#include <lz4.h>

#include "rvl.h"

#include "detail/rvl_p.h"
#include "detail/rvl_text_p.h"

static void rvl_read_chunk_header (RVL *self, RVLSize *size,
                                   RVLChunkCode *code);
static void rvl_read_chunk_payload (RVL *self, RVLByte *data, RVLSize size);

static void rvl_read_VHDR_chunk (RVL *self, RVLConstByte *rbuf, RVLSize size);
static void rvl_read_GRID_chunk (RVL *self, RVLConstByte *rbuf, RVLSize size);
static void rvl_read_DATA_chunk (RVL *self, RVLConstByte *rbuf, RVLSize size);
static void rvl_read_TEXT_chunk (RVL *self, RVLConstByte *rbuf, RVLSize size);

static void rvl_read_file_sig (RVL *self);
static void rvl_fread (RVL *self, RVLByte *data, RVLSize size);

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
          {
            rvl_alloc (self, &self->grid.dimBuf, size - 14);
            rvl_read_chunk_payload (self, rbuf, size);
            rvl_read_GRID_chunk (self, rbuf, size);
          }
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
          {
            rvl_alloc (self, &self->grid.dimBuf, size - 14);
            rvl_read_chunk_payload (self, rbuf, size);
            rvl_read_GRID_chunk (self, rbuf, size);
          }
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

void
rvl_read_chunk_header (RVL *self, RVLSize *size, RVLChunkCode *code)
{
  rvl_fread (self, (RVLByte *)size, 4);
  rvl_fread (self, (RVLByte *)code, 4);
}

void
rvl_read_chunk_payload (RVL *self, RVLByte *data, RVLSize size)
{
  rvl_fread (self, data, size);
}

void
rvl_read_VHDR_chunk (RVL *self, RVLConstByte *rbuf, RVLSize size)
{
  memcpy (&self->resolution, &rbuf[2], 12);
  memcpy (&self->primitive, &rbuf[14], 2);
  self->endian = rbuf[15];

  self->data.size = rvl_get_data_nbytes (self);
}

void
rvl_read_GRID_chunk (RVL *self, RVLConstByte *rbuf, RVLSize size)
{
  RVLSize  offset = 14;
  RVLGrid *grid   = &self->grid;

  grid->type = rbuf[0];
  grid->unit = rbuf[1];
  memcpy (grid->position, &rbuf[2], 12);

  RVLSize szdx = 0, szdy = 0, szdz = 0;
  if (grid->type == RVLGridType_Regular)
    {
      grid->ndx = 1;
      grid->ndy = 1;
      grid->ndz = 1;
      szdx = szdy = szdz = sizeof (f32);
    }
  else if (grid->type == RVLGridType_Rectilinear)
    {
      u32 *r    = self->resolution;
      grid->ndx = r[0];
      grid->ndy = r[1];
      grid->ndz = r[2];
      szdx      = r[0] * sizeof (f32);
      szdy      = r[1] * sizeof (f32);
      szdz      = r[2] * sizeof (f32);
    }

  grid->dx = (float *)(grid->dimBuf);
  grid->dy = (float *)(grid->dimBuf + szdx);
  grid->dz = (float *)(grid->dimBuf + szdx + szdy);
  memcpy (grid->dx, &rbuf[offset], szdx);
  memcpy (grid->dy, &rbuf[offset + szdx], szdy);
  memcpy (grid->dz, &rbuf[offset + szdx + szdy], szdz);
}

void
rvl_read_DATA_chunk (RVL *self, RVLConstByte *rbuf, RVLSize size)
{
  char *const   src     = (char *)rbuf;
  char *const   dst     = (char *)self->data.rbuf;
  const RVLSize srcSize = size;
  const RVLSize dstCap  = self->data.size;

  const RVLSize numBytes = LZ4_decompress_safe (src, dst, srcSize, dstCap);

  if (numBytes != self->data.size)
    {
      log_fatal ("[librvl read] Data decompression error!");
      exit (EXIT_FAILURE);
    }
}

void
rvl_read_TEXT_chunk (RVL *self, RVLConstByte *rbuf, RVLSize size)
{
  RVLText *newArr  = rvl_text_create_array (self->numText + 1);
  int      numText = self->numText + 1;

  if (self->text != NULL)
    {
      memcpy (newArr, self->text, sizeof (RVLText) * self->numText);
      free (self->text);
    }

  RVLText *newText = &newArr[numText - 1];

  // Both the key and the value will be null-terminated.
  RVLSize nullPos = 0;
  for (RVLSize i = 0; i < size; i++)
    {
      if (rbuf[i] == '\0')
        {
          nullPos = i;
        }
    }
  memcpy (newText->key, rbuf, nullPos + 1);

  RVLSize valueSize = size - (nullPos + 1);
  newText->value    = (char *)malloc (valueSize + 1);
  memcpy (newText->value, rbuf + nullPos + 1, valueSize);
  newText->value[valueSize + 1] = '\0';

  self->text    = newArr;
  self->numText = numText;
}

void
rvl_read_file_sig (RVL *self)
{
  RVLByte sig[RVL_FILE_SIG_SIZE];
  rvl_fread (self, sig, RVL_FILE_SIG_SIZE);

  for (RVLSize i = 0; i < RVL_FILE_SIG_SIZE; i++)
    {
      if (sig[i] != RVL_FILE_SIG[i])
        {
          log_fatal ("[librvl read] Not an RVL file.");
          exit (EXIT_FAILURE);
        }
    }
}

void
rvl_fread (RVL *self, RVLByte *data, RVLSize size)
{
  if (self->readFn == NULL)
    {
      log_fatal ("[librvl read] Call to NULL read function. Please check if "
                 "the RVL instance is a reader.");
      exit (EXIT_FAILURE);
    }

  self->readFn (self, data, size);
}

void
rvl_fread_default (RVL *self, RVLByte *data, RVLSize size)
{
  if (self->io == NULL)
    {
      log_fatal ("[librvl read] RVL is not intialized.");
      exit (EXIT_FAILURE);
    }

  const size_t count = fread (data, 1, size, self->io);

  if (count != size)
    {
      log_fatal ("[librvl read] fread failure.");
      exit (EXIT_FAILURE);
    }
}
