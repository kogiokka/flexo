#ifndef RVL_COMPRESS_H
#define RVL_COMPRESS_H

#ifndef RVL_H_INTERNAL
#error Never include this file directly. Use <rvl.h> instead.
#endif

#include "rvl_p.h"

void rvl_compress_lzma (RVL *self, BYTE **out, u32 *size);
void rvl_decompress_lzma (RVL *self, const BYTE *in, u32 size);

void rvl_compress_lz4 (RVL *self, BYTE **out, u32 *size);
void rvl_decompress_lz4 (RVL *self, const BYTE *in, u32 size);

#endif
