#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <log.h>

#include "rvl.h"

#include "detail/rvl_p.h"

#include "detail/rvl_compress_p.h"
#include "detail/rvl_text_p.h"

static void rvl_read_chunk_header (RVL *self, u32 *size, RVLChunkCode *code);
static void rvl_read_chunk_payload (RVL *self, BYTE *data, u32 size);

static void rvl_read_VFMT_chunk (RVL *self, const BYTE *rbuf, u32 size);
static void rvl_read_GRID_chunk (RVL *self, const BYTE *rbuf, u32 size);
static void rvl_read_DATA_chunk (RVL *self, const BYTE *rbuf, u32 size);
static void rvl_read_TEXT_chunk (RVL *self, const BYTE *rbuf, u32 size);

static void rvl_read_file_sig (RVL *self);
static void rvl_fread (RVL *self, BYTE *data, u32 size);

void
rvl_read_rvl (RVL *self)
{
  if (self == NULL)
    return;

  rvl_read_file_sig (self);

  RVLChunkCode code;
  do
    {
      u32 size;
      rvl_read_chunk_header (self, &size, &code);

      char *ch = (char *)&code;
      log_debug ("[librvl read] Reading chunk header: size %d, code: %c%c%c%c",
                 size, ch[0], ch[1], ch[2], ch[3]);

      BYTE *rbuf = (BYTE *)malloc (size);
      switch (code)
        {
        case RVL_CHUNK_CODE_VFMT:
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_VFMT_chunk (self, rbuf, size);
          break;
        case RVL_CHUNK_CODE_GRID:
          {
            rvl_alloc (self, &self->grid.dimBuf, size - 14);
            rvl_read_chunk_payload (self, rbuf, size);
            rvl_read_GRID_chunk (self, rbuf, size);
          }
          break;
        case RVL_CHUNK_CODE_DATA:
          rvl_alloc (self, &self->data.rbuf, self->data.size);
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_DATA_chunk (self, rbuf, size);
          break;
        case RVL_CHUNK_CODE_TEXT:
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_TEXT_chunk (self, rbuf, size);
          break;
        case RVL_CHUNK_CODE_VEND:
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
  while (code != RVL_CHUNK_CODE_VEND);
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
      u32 size;
      rvl_read_chunk_header (self, &size, &code);

      BYTE *rbuf = (BYTE *)malloc (size);
      switch (code)
        {
        case RVL_CHUNK_CODE_VFMT:
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_VFMT_chunk (self, rbuf, size);
          break;
        case RVL_CHUNK_CODE_GRID:
          {
            rvl_alloc (self, &self->grid.dimBuf, size - 14);
            rvl_read_chunk_payload (self, rbuf, size);
            rvl_read_GRID_chunk (self, rbuf, size);
          }
          break;
        case RVL_CHUNK_CODE_TEXT:
          rvl_read_chunk_payload (self, rbuf, size);
          rvl_read_TEXT_chunk (self, rbuf, size);
          break;
        default:
          fseek (self->io, size, SEEK_CUR);
          break;
        }
      free (rbuf);
    }
  while (code != RVL_CHUNK_CODE_VEND);
  fseek (self->io, RVL_FILE_SIG_SIZE, SEEK_SET);
}

void
rvl_read_data_buffer (RVL *self, void **buffer)
{
  if (self == NULL)
    return;

  self->data.rbuf = *buffer;

  RVLChunkCode code;
  do
    {
      u32 size;
      rvl_read_chunk_header (self, &size, &code);

      BYTE *rbuf = (BYTE *)malloc (size);

      if (code == RVL_CHUNK_CODE_DATA)
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
  while (code != RVL_CHUNK_CODE_VEND);

  self->data.rbuf = NULL;
  fseek (self->io, RVL_FILE_SIG_SIZE, SEEK_SET);
}

void
rvl_read_chunk_header (RVL *self, u32 *size, RVLChunkCode *code)
{
  rvl_fread (self, (BYTE *)size, 4);
  rvl_fread (self, (BYTE *)code, 4);
}

void
rvl_read_chunk_payload (RVL *self, BYTE *data, u32 size)
{
  rvl_fread (self, data, size);
}

void
rvl_read_VFMT_chunk (RVL *self, const BYTE *rbuf, u32 size)
{
  RVLPrimitive primitive;
  RVLEndian    endian;
  RVLCompress  compress;

  memcpy (&self->resolution, &rbuf[2], 12);
  memcpy (&primitive, &rbuf[14], 2);
  memcpy (&endian, &rbuf[16], 1);
  memcpy (&compress, &rbuf[17], 1);

  self->primitive = primitive;
  self->endian    = endian;
  self->compress  = compress;

  self->data.size = rvl_get_data_nbytes (self);
}

void
rvl_read_GRID_chunk (RVL *self, const BYTE *rbuf, u32 size)
{
  u32      offset = 14;
  RVLGrid *grid   = &self->grid;

  grid->type = rbuf[0];
  grid->unit = rbuf[1];
  memcpy (grid->position, &rbuf[2], 12);

  u32 szdx = 0, szdy = 0, szdz = 0;
  if (grid->type == RVL_GRID_REGULAR)
    {
      grid->ndx = 1;
      grid->ndy = 1;
      grid->ndz = 1;
      szdx = szdy = szdz = sizeof (f32);
    }
  else if (grid->type == RVL_GRID_RECTILINEAR)
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
rvl_read_DATA_chunk (RVL *self, const BYTE *rbuf, u32 size)
{
  if (self->compress == RVL_COMPRESSION_LZ4)
    {
      rvl_decompress_lz4 (self, rbuf, size);
    }
  else if (self->compress == RVL_COMPRESSION_LZMA)
    {
      rvl_decompress_lzma (self, rbuf, size);
    }
}

void
rvl_read_TEXT_chunk (RVL *self, const BYTE *rbuf, u32 size)
{
  RVLText *text = rvl_text_create ();

  RVLTag tag;
  memcpy (&tag, &rbuf[0], 1);
  text->tag = ((RVLenum)tag) | 0x0D00;

  u32 valueLen = size - 1;
  text->value  = (char *)malloc (valueLen + 1);

  text->value[valueLen] = '\0';
  memcpy (text->value, &rbuf[1], valueLen);

  log_trace ("[librvl read] Read TEXT: %.4X, %s", text->tag, text->value);

  if (self->text == NULL)
    {
      self->text = text;
      return;
    }

  RVLText *cur = self->text;
  while (cur->next != NULL)
    {
      cur = cur->next;
    }

  cur->next = text;
}

void
rvl_read_file_sig (RVL *self)
{
  BYTE sig[RVL_FILE_SIG_SIZE];
  rvl_fread (self, sig, RVL_FILE_SIG_SIZE);

  for (u32 i = 0; i < RVL_FILE_SIG_SIZE; i++)
    {
      if (sig[i] != RVL_FILE_SIG[i])
        {
          log_fatal ("[librvl read] Not an RVL file.");
          exit (EXIT_FAILURE);
        }
    }
}

void
rvl_fread (RVL *self, BYTE *data, u32 size)
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
rvl_fread_default (RVL *self, BYTE *data, u32 size)
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
