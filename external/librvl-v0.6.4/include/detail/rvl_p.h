#ifndef RVL_P_H
#define RVL_P_H

#ifndef RVL_H_INTERNAL
#error Never include this file directly. Use <rvl.h> instead.
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../rvl.h"

typedef uint8_t  BYTE;
typedef uint32_t u32;
typedef float    f32;
typedef uint8_t  u8;
typedef uint16_t u16;

/* Part of VFMT */
typedef uint16_t RVLPrimitive;
typedef uint8_t  RVLEndian;
typedef uint8_t  RVLCompress;

/* Part of GRID */
typedef uint8_t RVLGridType;
typedef int8_t  RVLGridUnit;

typedef struct RVLText RVLText;

typedef void (*RVLWriteFn) (RVL *, const BYTE *, u32);
typedef void (*RVLReadFn) (RVL *, BYTE *, u32);

typedef struct
{
  const BYTE *wbuf; // Non-owning pointer
  BYTE       *rbuf;
  u32         size;
} RVLData;

typedef struct
{
  RVLGridType type;
  RVLGridUnit unit;
  f32         position[3];

  /**
   * Voxel dimensions buffer
   *
   * The buffer pointer is owned by RVL struct.
   */
  BYTE *dimBuf;
  u32   dimBufSz;

  f32 *dx, *dy, *dz;
  u32  ndx, ndy, ndz;
} RVLGrid;

/**
 * Chunk code constants
 *
 * A chunk code is an unsigned integer that consists of human readable
 * characters. CHUNK_CODE macro puts the code characters into the integer in
 * little-endian order.
 */
#define SHIFT32(byte, numBits) ((u32)(byte) << numBits)
#define CHUNK_CODE(b1, b2, b3, b4)                                            \
  (SHIFT32 (b4, 24) | SHIFT32 (b3, 16) | SHIFT32 (b2, 8) | b1)

typedef uint32_t RVLChunkCode;

#define RVL_CHUNK_CODE_VFMT CHUNK_CODE (86, 70, 77, 84)
#define RVL_CHUNK_CODE_GRID CHUNK_CODE (71, 82, 73, 68)
#define RVL_CHUNK_CODE_DATA CHUNK_CODE (68, 65, 84, 65)
#define RVL_CHUNK_CODE_TEXT CHUNK_CODE (84, 69, 88, 84)
#define RVL_CHUNK_CODE_VEND CHUNK_CODE (86, 69, 78, 68)

// RVL File Signature: .RVL FORMAT\0
#define RVL_FILE_SIG_SIZE 12
extern BYTE RVL_FILE_SIG[RVL_FILE_SIG_SIZE];

typedef enum
{
  RVLIoState_Read,
  RVLIoState_Write,
} RVLIoState;

struct RVL
{
  bool       isOwningIo;
  FILE      *io;
  RVLIoState ioState;
  RVLWriteFn writeFn;
  RVLReadFn  readFn;

  /* VFMT chunk */
  u8           version[2]; // major, minor
  u32          resolution[3];
  RVLPrimitive primitive;
  RVLEndian    endian;
  RVLCompress  compress;

  /* GRID chunk */
  RVLGrid grid;

  /* DATA chunk */
  RVLData data;

  /* TEXT chunk */
  RVLText *text;
};

void rvl_alloc (RVL *self, BYTE **ptr, u32 size);
void rvl_dealloc (RVL *self, BYTE **ptr);
void rvl_fwrite_default (RVL *self, const BYTE *data, u32 size);
void rvl_fread_default (RVL *self, BYTE *data, u32 size);

#endif
