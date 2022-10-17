#ifndef RVL_P_H
#define RVL_P_H

#include <stdint.h>
#include <stdio.h>

#include "../rvl.h"

typedef float    f32;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef void (*RVLWriteFn) (RVL *, RVLConstByte *, RVLSize);
typedef void (*RVLReadFn) (RVL *, RVLByte *, RVLSize);

typedef struct
{
  RVLConstByte *wbuf; // Non-owning pointer
  RVLByte      *rbuf;
  RVLSize       size;
} RVLData;

typedef struct
{
  RVLGridType type;
  RVLGridUnit unit;
  f32         position[3];

  /**
   * Voxel dimensions buffer
   *
   * The buffer is managed by RVL struct.
   */
  RVLByte *vxDimBuf;
  RVLSize  vxDimBufSize;
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

#define RVLChunkCode_VHDR CHUNK_CODE (86, 72, 68, 82)
#define RVLChunkCode_GRID CHUNK_CODE (71, 82, 73, 68)
#define RVLChunkCode_DATA CHUNK_CODE (68, 65, 84, 65)
#define RVLChunkCode_VEND CHUNK_CODE (86, 69, 78, 68)

#define RVLChunkCode_TEXT CHUNK_CODE (84, 69, 88, 84)

// RVL File Signature: .RVL FORMAT\0
#define RVL_FILE_SIG_SIZE 12
extern RVLByte RVL_FILE_SIG[RVL_FILE_SIG_SIZE];

typedef enum
{
  RVLIoState_Read,
  RVLIoState_Write,
} RVLIoState;

struct RVL
{
  FILE      *io;
  RVLIoState ioState;
  RVLWriteFn writeFn;
  RVLReadFn  readFn;

  /* VHDR chunk */
  u8           version[2]; // major, minor
  u32          resolution[3];
  RVLPrimitive primitive;
  RVLEndian    endian;

  /* GRID chunk */
  RVLGrid grid;

  /* DATA chunk */
  RVLData data;

  /* TEXT chunk */
  RVLText *text;
  int      numText;
};

RVLText *rvl_text_create_array (int num);
void     rvl_text_destroy_array (RVLText **self);
void     rvl_alloc (RVL *self, RVLByte **ptr, RVLSize size);
void     rvl_dealloc (RVL *self, RVLByte **ptr);
void     rvl_fwrite_default (RVL *self, RVLConstByte *data, RVLSize size);
void     rvl_fread_default (RVL *self, RVLByte *data, RVLSize size);

#endif
