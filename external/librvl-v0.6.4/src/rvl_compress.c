#include <stdbool.h>
#include <stdlib.h>

#include <lz4.h>
#include <lz4hc.h>
#include <lzma.h>

#include "detail/rvl_compress_p.h"
#include "detail/rvl_log_p.h"

static void create_lzma_encoder (lzma_stream *self, uint32_t preset);
static void create_lzma_decoder (lzma_stream *strm);
static void destroy_lzma_coder (lzma_stream *self);

static lzma_ret run_lzma_compression (lzma_stream *strm, const BYTE *src,
                                      u32 srcSize, BYTE *dst, u32 dstCap);
static lzma_ret run_lzma_decompression (lzma_stream *strm, const BYTE *src,
                                        u32 srcSize, BYTE **dst, u32 dstCap);

static void print_lzma_compression_error (lzma_ret ret);
static void print_lzma_decompression_error (lzma_ret ret);

void
rvl_compress_lzma (RVL *self, BYTE **out, u32 *size)
{
  lzma_stream strm;

  const BYTE *src     = self->data.wbuf;
  u32         srcSize = self->data.size;
  u32         dstCap  = self->data.size;

  rvl_log_debug ("Starting XZ compression. The original size is %u bytes.",
                 srcSize);

  lzma_ret ret;
  create_lzma_encoder (&strm, 6 | LZMA_PRESET_DEFAULT);
  ret = run_lzma_compression (&strm, src, srcSize, *out, dstCap);

  if (ret != LZMA_STREAM_END)
    {
      destroy_lzma_coder (&strm);
      create_lzma_encoder (&strm, 6 | LZMA_PRESET_DEFAULT);

      dstCap = lzma_stream_buffer_bound (self->data.size);
      *out   = realloc (*out, dstCap);

      rvl_log_debug ("Reallocate output memory to %u bytes.", dstCap);

      ret = run_lzma_compression (&strm, src, srcSize, *out, dstCap);
    }

  if (ret != LZMA_STREAM_END)
    {
      print_lzma_compression_error (ret);
      exit (EXIT_FAILURE);
    }

  *size = strm.total_out;
  destroy_lzma_coder (&strm);

  rvl_log_debug ("Compression succeeded. The result has %u bytes.",
                 strm.total_out);
}

void
rvl_decompress_lzma (RVL *self, const BYTE *in, u32 size)
{
  lzma_stream strm;

  lzma_ret ret;
  create_lzma_decoder (&strm);
  ret = run_lzma_decompression (&strm, in, size, &self->data.rbuf,
                                self->data.size);

  if (ret != LZMA_STREAM_END)
    {
      print_lzma_decompression_error (ret);
      exit (EXIT_FAILURE);
    }

  if (strm.total_out != self->data.size)
    {
      rvl_log_fatal ("Decompression failed. The original data size should be "
                     "%u. However, the decompressed size was %u.",
                     self->data.size, strm.total_out);
      exit (EXIT_FAILURE);
    }
  destroy_lzma_coder (&strm);

  rvl_log_debug ("Decompression succeeded. The result has %u bytes.",
                 strm.total_out);
}

void
rvl_compress_lz4 (RVL *self, BYTE **out, u32 *size)
{
  const char *src     = (const char *)self->data.wbuf;
  u32         srcSize = self->data.size;
  char       *dst     = (char *)*out;
  u32         dstCap  = self->data.size;

  rvl_log_debug ("Starting LZ4 compression. The original size is %u bytes.",
                 srcSize);

  int nbytes = LZ4_compress_HC (src, dst, srcSize, dstCap, LZ4HC_CLEVEL_MIN);

  // When the compressed size is greater than the uncompressed one.
  if (nbytes == 0)
    {
      dstCap = LZ4_compressBound (self->data.size);
      *out   = realloc (*out, srcSize);
      dst    = (char *)*out;

      nbytes = LZ4_compress_HC (src, dst, srcSize, dstCap, LZ4HC_CLEVEL_MIN);
    }

  *size = nbytes;

  rvl_log_debug ("Compression succeeded. The result has %u bytes.", nbytes);
}

void
rvl_decompress_lz4 (RVL *self, const BYTE *in, u32 size)
{
  const char *src     = (const char *)in;
  u32         srcSize = size;
  char       *dst     = (char *)self->data.rbuf;
  u32         dstCap  = self->data.size;

  int nbytes = LZ4_decompress_safe (src, dst, srcSize, dstCap);
  if ((u32)nbytes != self->data.size)
    {
      rvl_log_fatal (
          "Decompression failed. The returned number of bytes is %d.", nbytes);
      exit (EXIT_FAILURE);
    }

  rvl_log_debug ("Decompression succeeded. The result has %u bytes.", nbytes);
}

void
create_lzma_encoder (lzma_stream *self, uint32_t preset)
{
  *self = (lzma_stream)LZMA_STREAM_INIT;

  lzma_ret ret = lzma_easy_encoder (self, preset, LZMA_CHECK_CRC64);

  // Return successfully
  if (ret == LZMA_OK)
    return;

  print_lzma_compression_error (ret);
}

void
create_lzma_decoder (lzma_stream *self)
{
  *self = (lzma_stream)LZMA_STREAM_INIT;

  lzma_ret ret = lzma_stream_decoder (self, UINT32_MAX, LZMA_CONCATENATED);

  // Return successfully
  if (ret == LZMA_OK)
    return;

  print_lzma_decompression_error (ret);
}

void
destroy_lzma_coder (lzma_stream *self)
{
  lzma_end (self);
}

lzma_ret
run_lzma_compression (lzma_stream *strm, const BYTE *src, u32 srcSize,
                      BYTE *dst, u32 dstCap)
{
  strm->next_in   = src;
  strm->avail_in  = srcSize;
  strm->next_out  = dst;
  strm->avail_out = dstCap;

  return lzma_code (strm, LZMA_FINISH);
}

lzma_ret
run_lzma_decompression (lzma_stream *strm, const BYTE *src, u32 srcSize,
                        BYTE **dst, u32 dstCap)
{
  strm->next_in   = src;
  strm->avail_in  = srcSize;
  strm->next_out  = *dst;
  strm->avail_out = dstCap;

  return lzma_code (strm, LZMA_FINISH);
}

void
print_lzma_compression_error (lzma_ret ret)
{
  const char *msg;
  switch (ret)
    {
    case LZMA_MEM_ERROR:
      msg = "Memory allocation failed";
      break;
    case LZMA_DATA_ERROR:
      msg = "File size limits exceeded";
      break;
    case LZMA_OPTIONS_ERROR:
      msg = "Specified preset is not supported";
      break;
    case LZMA_UNSUPPORTED_CHECK:
      msg = "Specified integrity check is not supported";
      break;
    default:
      msg = "Unknown error, possibly a bug";
      break;
    }

  rvl_log_error ("Compression failed: %s (error code %u)\n", msg, ret);
}

void
print_lzma_decompression_error (lzma_ret ret)
{
  const char *msg;
  switch (ret)
    {
    case LZMA_MEM_ERROR:
      msg = "Memory allocation failed";
      break;
    case LZMA_FORMAT_ERROR:
      msg = "The input is not in the .xz format";
      break;
    case LZMA_OPTIONS_ERROR:
      msg = "Unsupported compression options";
      break;
    case LZMA_DATA_ERROR:
      msg = "Compressed file is corrupt";
      break;
    case LZMA_BUF_ERROR:
      msg = "Compressed file is truncated or "
            "otherwise corrupt";
      break;
    default:
      msg = "Unknown error, possibly a bug";
      break;
    }

  rvl_log_error ("Decompression failed: %s (error code %u)\n", msg, ret);
}
