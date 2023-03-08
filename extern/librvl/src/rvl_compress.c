#include <stdbool.h>
#include <stdlib.h>

#include <lz4.h>
#include <lz4hc.h>
#include <lzma.h>

#include "detail/rvl_compress_p.h"
#include "detail/rvl_log_p.h"

lzma_filter *get_lzma_default_filters ();

static void print_lzma_compression_error (lzma_ret ret);
static void print_lzma_decompression_error (lzma_ret ret);

void
rvl_compress_lzma (RVL *self, BYTE **out, u32 *size)
{
  const BYTE *src     = self->data.wbuf;
  u32         srcSize = self->data.size;
  u32         dstCap  = self->data.size;

  rvl_log_debug ("Starting XZ compression. The original size is %u bytes.",
                 srcSize);

  lzma_filter *filters = get_lzma_default_filters ();

  lzma_ret ret;
  size_t   outPos = 0;
  ret = lzma_raw_buffer_encode (filters, NULL, src, srcSize, *out, &outPos,
                                dstCap);

  // Handle tiny data
  if (ret == LZMA_BUF_ERROR)
    {
      dstCap = 256;
      *out   = realloc (*out, dstCap);

      rvl_log_debug ("Reallocate output memory to %u bytes.", dstCap);
      ret = lzma_raw_buffer_encode (filters, NULL, src, srcSize, *out, &outPos,
                                    dstCap);
    }

  if (ret != LZMA_OK)
    {
      print_lzma_compression_error (ret);
      exit (EXIT_FAILURE);
    }

  *size = outPos;
  rvl_log_debug ("Compression succeeded. The result has %u bytes.", outPos);
}

void
rvl_decompress_lzma (RVL *self, const BYTE *in, u32 size)
{
  BYTE *out    = self->data.rbuf;
  u32   dstCap = self->data.size;

  lzma_filter *filters = get_lzma_default_filters ();

  lzma_ret ret;
  size_t   inPos  = 0;
  size_t   outPos = 0;
  ret = lzma_raw_buffer_decode (filters, NULL, in, &inPos, size, out, &outPos,
                                dstCap);

  if (ret != LZMA_OK)
    {
      print_lzma_decompression_error (ret);
      exit (EXIT_FAILURE);
    }

  if (outPos != self->data.size)
    {
      rvl_log_fatal ("Decompression failed. The original data size should be "
                     "%u. However, the decompressed size was %u.",
                     self->data.size, outPos);
      exit (EXIT_FAILURE);
    }

  rvl_log_debug ("Decompression succeeded. The result has %u bytes.", outPos);
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

      rvl_log_debug ("Reallocate output memory to %u bytes.", dstCap);

      *out   = realloc (*out, dstCap);
      dst    = (char *)*out;
      nbytes = LZ4_compress_HC (src, dst, srcSize, dstCap, LZ4HC_CLEVEL_MIN);
    }

  if (nbytes <= 0)
    {
      rvl_log_fatal ("LZ4 compression failed.");
      exit (EXIT_FAILURE);
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

lzma_filter *
get_lzma_default_filters ()
{
  static bool initialized = false;

  static lzma_filter       filters[LZMA_FILTERS_MAX + 1];
  static lzma_options_lzma options;

  if (initialized)
    {
      return filters;
    }

  options.preset_dict      = NULL;
  options.preset_dict_size = 0;

  options.lc        = LZMA_LC_DEFAULT;
  options.lp        = LZMA_LP_DEFAULT;
  options.pb        = LZMA_PB_DEFAULT;
  options.dict_size = 1U << 23;
  options.mode      = LZMA_MODE_NORMAL;
  options.mf        = LZMA_MF_BT4;
  options.nice_len  = 64;
  options.depth     = 0;

  filters[0].id      = LZMA_FILTER_LZMA2;
  filters[0].options = &options;
  filters[1].id      = LZMA_VLI_UNKNOWN;
  filters[1].options = NULL;

  initialized = true;

  return filters;
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
                        BYTE *dst, u32 dstCap)
{
  strm->next_in   = src;
  strm->avail_in  = srcSize;
  strm->next_out  = dst;
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
