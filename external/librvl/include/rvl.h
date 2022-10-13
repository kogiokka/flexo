/**
 * rvl.h - Structs and functions for read/write an rvl file.
 *
 * Chunk
 * =====
 *
 * Each chunk consists of three parts: size, code and payload.
 *
 * +00 4B chunk size (N bytes)
 * +04 4B chunk code
 * +08 NB chunk payload
 *
 * Chunk Codes
 * ===========
 *
 * Currently, there are 4 types of chunk: VHDR, DATA, TEXT, VEND.
 *
 * VHDR Chunk
 * ++++++++++
 *
 * VHDR chunk holds the information on how to interpret the DATA chunk. The
 * layout of the chunk payload is as follow:
 *
 * +00 1B    librvl major version
 * +01 1B    librvl minor version
 * +02 1B    grid type
 * +03 1B    grid unit
 * +04 2B    primitive
 * +06 1B    endianness
 * +07 1B    padding byte
 * +08 12B   resolution
 * +20 12B   voxel size
 * +32 12B   coordinates of the lower corner of the dataset
 *
 * Total 44 bytes
 *
 */

#ifndef RVL_H
#define RVL_H

#include <stdint.h>

#define RVL_VERSION_MAJOR 0
#define RVL_VERSION_MINOR 1

typedef struct RVL     RVL;
typedef struct RVLText RVLText;

typedef uint8_t       RVLByte;
typedef const uint8_t RVLConstByte;
typedef uint32_t      RVLSize;

typedef uint16_t RVLPrimitive;
typedef uint8_t  RVLGridType;
typedef int8_t   RVLGridUnit;
typedef uint8_t  RVLEndian;

#define RVLPrimitive_u8   0x0103
#define RVLPrimitive_u16  0x0104
#define RVLPrimitive_u32  0x0105
#define RVLPrimitive_u64  0x0106
#define RVLPrimitive_u128 0x0107

#define RVLPrimitive_i8   0x0113
#define RVLPrimitive_i16  0x0114
#define RVLPrimitive_i32  0x0115
#define RVLPrimitive_i64  0x0116
#define RVLPrimitive_i128 0x0117

#define RVLPrimitive_f8   0x0123
#define RVLPrimitive_f16  0x0124
#define RVLPrimitive_f32  0x0125
#define RVLPrimitive_f64  0x0126
#define RVLPrimitive_f128 0x0127

#define RVLPrimitive_vec2u8   0x0203
#define RVLPrimitive_vec2u16  0x0204
#define RVLPrimitive_vec2u32  0x0205
#define RVLPrimitive_vec2u64  0x0206
#define RVLPrimitive_vec2u128 0x0207

#define RVLPrimitive_vec2i8   0x0213
#define RVLPrimitive_vec2i16  0x0214
#define RVLPrimitive_vec2i32  0x0215
#define RVLPrimitive_vec2i64  0x0216
#define RVLPrimitive_vec2i128 0x0217

#define RVLPrimitive_vec2f8   0x0223
#define RVLPrimitive_vec2f16  0x0224
#define RVLPrimitive_vec2f32  0x0225
#define RVLPrimitive_vec2f64  0x0226
#define RVLPrimitive_vec2f128 0x0227

#define RVLPrimitive_vec3u8   0x0303
#define RVLPrimitive_vec3u16  0x0304
#define RVLPrimitive_vec3u32  0x0305
#define RVLPrimitive_vec3u64  0x0306
#define RVLPrimitive_vec3u128 0x0307

#define RVLPrimitive_vec3i8   0x0313
#define RVLPrimitive_vec3i16  0x0314
#define RVLPrimitive_vec3i32  0x0315
#define RVLPrimitive_vec3i64  0x0316
#define RVLPrimitive_vec3i128 0x0317

#define RVLPrimitive_vec3f8   0x0323
#define RVLPrimitive_vec3f16  0x0324
#define RVLPrimitive_vec3f32  0x0325
#define RVLPrimitive_vec3f64  0x0326
#define RVLPrimitive_vec3f128 0x0327

#define RVLPrimitive_vec4u8   0x0403
#define RVLPrimitive_vec4u16  0x0404
#define RVLPrimitive_vec4u32  0x0405
#define RVLPrimitive_vec4u64  0x0406
#define RVLPrimitive_vec4u128 0x0407

#define RVLPrimitive_vec4i8   0x0413
#define RVLPrimitive_vec4i16  0x0414
#define RVLPrimitive_vec4i32  0x0415
#define RVLPrimitive_vec4i64  0x0416
#define RVLPrimitive_vec4i128 0x0417

#define RVLPrimitive_vec4f8   0x0423
#define RVLPrimitive_vec4f16  0x0424
#define RVLPrimitive_vec4f32  0x0425
#define RVLPrimitive_vec4f64  0x0426
#define RVLPrimitive_vec4f128 0x0427

#define RVLEndian_Big    0
#define RVLEndian_Little 1

#define RVLGridType_Cartesian   0
#define RVLGridType_Regular     1
#define RVLGridType_Rectilinear 2

// -128 - N/A or unknown, n - 1.0e+n meter
#define RVLGridUnit_NA         -128
#define RVLGridUnit_Milimeter  -3
#define RVLGridUnit_Centimeter -2
#define RVLGridUnit_Meter      0
#define RVLGridUnit_Kilometer  3

// Prevent name-mangling for librvl functions from C++ compiler.
#ifdef __cplusplus
extern "C"
{
#endif

  RVL *rvl_create_writer (const char *filename);
  RVL *rvl_create_reader (const char *filename);
  void rvl_destroy (RVL **self);

  // Write the entire rvl file
  void rvl_write_rvl (RVL *self);

  // Read the entire rvl file
  void rvl_read_rvl (RVL *self);

  // Read the information without the actual volumetric data
  // Use rvl_get_* to retrieve information.
  void rvl_read_info (RVL *self);

  // Read the entire volumetric data into a 1D buffer. The data allocation and
  // deallocation is managed by the user.
  void rvl_read_data_buffer (RVL *self, RVLByte **data);

  void rvl_set_grid_type (RVL *self, RVLGridType gridType);
  void rvl_set_grid_unit (RVL *self, RVLGridUnit gridUnit);
  void rvl_set_primitive (RVL *self, RVLPrimitive primitive);
  void rvl_set_endian (RVL *self, RVLEndian endian);

  void rvl_set_resolution (RVL *self, int x, int y, int z);
  void rvl_set_voxel_size (RVL *self, float x, float y, float z);
  void rvl_set_position (RVL *self, float x, float y, float z);

  RVLGridType  rvl_get_grid_type (RVL *self);
  RVLGridUnit  rvl_get_grid_unit (RVL *self);
  RVLPrimitive rvl_get_primitive (RVL *self);
  RVLEndian    rvl_get_endian (RVL *self);

  void rvl_get_resolution (RVL *self, int *x, int *y, int *z);
  void rvl_get_voxel_size (RVL *self, float *x, float *y, float *z);
  void rvl_get_position (RVL *self, float *x, float *y, float *z);

  RVLSize rvl_get_primitive_byte_count (RVL *self);

  RVLText *rvl_text_create_array (int num);
  void     rvl_text_destroy_array (RVLText **self);
  void     rvl_set_text (RVL *self, RVLText **text, int numText);
  void     rvl_get_text (RVL *self, RVLText **text, int *numText);
  void     rvl_text_set (RVLText *textArr, int index, char *key, char *value);
  void     rvl_text_get (RVLText *textArr, int index, const char **key,
                         const char **value);

  void rvl_alloc_data_buffer (RVL *self, RVLByte **buffer, RVLSize *size);
  void rvl_dealloc_data_buffer (RVL *self, RVLByte **buffer);
  void rvl_set_data_buffer (RVL *self, RVLConstByte *buffer, RVLSize size);
  void rvl_get_data_buffer (RVL *self, RVLByte **buffer, RVLSize *size);

#ifdef __cplusplus
}
#endif

#endif
