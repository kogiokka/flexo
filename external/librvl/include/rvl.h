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
 * +04 1B    data format
 * +05 1B    bit depth
 * +06 1B    data dimensions
 * +07 1B    endianness
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

typedef struct RVL RVL;
typedef struct RVLText RVLText;

typedef uint8_t RVLByte;
typedef const uint8_t RVLConstByte;
typedef uint32_t RVLSize;

typedef uint8_t RVLValueFormat;
typedef uint8_t RVLValueBitDepth;
typedef uint8_t RVLValueDimen;
typedef uint8_t RVLGridType;
typedef int8_t RVLGridUnit;
typedef uint8_t RVLEndian;

#define RVLValueFormat_Unsigned 0
#define RVLValueFormat_Signed 1
#define RVLValueFormat_IEEE754 2

#define RVLValueDimen_Scalar 1
#define RVLValueDimen_Vec2 2
#define RVLValueDimen_Vec3 3
#define RVLValueDimen_Vec4 4

#define RVLValueBitDepth_8 8
#define RVLValueBitDepth_16 16
#define RVLValueBitDepth_32 32
#define RVLValueBitDepth_64 64
#define RVLValueBitDepth_128 128

#define RVLEndian_Big 0
#define RVLEndian_Little 1

#define RVLGridType_Cartesian 0
#define RVLGridType_Regular 1
#define RVLGridType_Rectilinear 2

// -128 - N/A or unknown, n - 1.0e+n meter
#define RVLGridUnit_NA -128
#define RVLGridUnit_Milimeter -3
#define RVLGridUnit_Centimeter -2
#define RVLGridUnit_Meter 0
#define RVLGridUnit_Kilometer 3

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
  void rvl_set_value_format (RVL *self, RVLValueFormat format);
  void rvl_set_value_dimension (RVL *self, RVLValueDimen dimen);
  void rvl_set_value_bit_depth (RVL *self, RVLValueBitDepth bitDepth);
  void rvl_set_endian (RVL *self, RVLEndian endian);

  void rvl_set_resolution (RVL *self, int x, int y, int z);
  void rvl_set_voxel_size (RVL *self, float x, float y, float z);
  void rvl_set_position (RVL *self, float x, float y, float z);

  RVLGridType rvl_get_grid_type (RVL *self);
  RVLGridUnit rvl_get_grid_unit (RVL *self);
  RVLValueDimen rvl_get_value_dimension (RVL *self);
  RVLValueFormat rvl_get_value_format (RVL *self);
  RVLValueBitDepth rvl_get_value_bit_depth (RVL *self);
  RVLEndian rvl_get_endian (RVL *self);

  void rvl_get_resolution (RVL *self, int *x, int *y, int *z);
  void rvl_get_voxel_size (RVL *self, float *x, float *y, float *z);
  void rvl_get_position (RVL *self, float *x, float *y, float *z);

  RVLSize rvl_get_value_byte_count (RVL *self);

  RVLText *rvl_text_create_array (int num);
  void rvl_text_destroy_array (RVLText **self);
  void rvl_set_text (RVL *self, RVLText **text, int numText);
  void rvl_get_text (RVL *self, RVLText **text, int *numText);
  void rvl_text_set (RVLText *textArr, int index, char *key, char *value);
  void rvl_text_get (RVLText *textArr, int index, const char **key,
                     const char **value);

  void rvl_alloc_data_buffer (RVL *self, RVLByte **buffer, RVLSize *size);
  void rvl_dealloc_data_buffer (RVL *self, RVLByte **buffer);
  void rvl_set_data_buffer (RVL *self, RVLConstByte *buffer, RVLSize size);
  void rvl_get_data_buffer (RVL *self, RVLByte **buffer, RVLSize *size);

#ifdef __cplusplus
}
#endif

#endif
