/*******************************************************************************
   rvl.h - Structs and functions for read/write an rvl file.

   RVL Specification
   *****************

   Chunk
   =====

   Chunk is the basic structure to hold RVL information. Each chunk consists of
   three parts: size, code and payload:

       +00 4B chunk size (N bytes)
       +04 4B chunk code
       +08 NB chunk payload
       +xx 4B CRC32

   There are 5 types of chunk: VFMT, GRID, DATA, TEXT, VEND. A valid RVL file
   must contain a VFMT chunk, a GRID chunk, one or more DATA chunks, and a VEND
   chunk.

   VFMT Chunk
   ++++++++++

       +00 1B    librvl major version
       +01 1B    librvl minor version
       +02 12B   resolution
       +14 2B    primitive
       +16 1B    endian
       +17 1B    compression method

   VFMT chunk holds the information on how to interpret the DATA chunk.


   GRID Chunk
   ++++++++++

       +00   1B   grid type
       +01   1B   grid unit
       +02  12B   position of the lower corner of the grid
       +14   nB   voxel dimensions

   GRID chunk describes how should one present the volumetric data. There are
   two types of grid: Regular and Rectilinear. Each grid type has its own voxel
   dimensions configuration.

   Regular Grid
   ------------

       +14  12B   three values for x, y, and z, respectively.

   Rectilinear Grid
   ----------------

       +14 [voxel dimensions in x-direction]
       +xx [voxel dimensions in y-direction]
       +xx [voxel dimensions in z-direction]


   TEXT Chunk
   ++++++++++

   A field of TEXT consists of a tag and its value. The tags and the information
   that each field should hold are part of the specification.

   +---------------+-----------------------------------------------------------+
   | Tag           | Value                                                     |
   +===============+===========================================================+
   | Title         | Data name or title.                                       |
   +---------------+-----------------------------------------------------------+
   | Description   | Data description.                                         |
   +---------------+-----------------------------------------------------------+
   | Author        | Name of the creator(s).                                   |
   +---------------+-----------------------------------------------------------+
   | Copyright     | Copyright notice.                                         |
   +---------------+-----------------------------------------------------------+
   | License       | Legal document to grant additional copyright permissions  |
   |               | to the general public. (ex. Creative Common licenses).    |
   +---------------+-----------------------------------------------------------+
   | Source        | Source of the data (ex. place, device, etc.)              |
   +---------------+-----------------------------------------------------------+
   | Creation Time | Creation time of original data. The text should be in ISO |
   |               | 8601 format (ex. YYYY-MM-DDThh:mm:ssTZD)                  |
   +---------------+-----------------------------------------------------------+

*******************************************************************************/

#ifndef RVL_H
#define RVL_H

#include <stdio.h>

#define RVL_VERSION_MAJOR 0
#define RVL_VERSION_MINOR 7

/* RVL struct types */
typedef struct RVL RVL;

/* enum types */
typedef unsigned int RVLenum;

/* enum definitions */
#define RVL_PRIMITIVE_U8   0x0103
#define RVL_PRIMITIVE_U16  0x0104
#define RVL_PRIMITIVE_U32  0x0105
#define RVL_PRIMITIVE_U64  0x0106
#define RVL_PRIMITIVE_U128 0x0107
#define RVL_PRIMITIVE_I8   0x0113
#define RVL_PRIMITIVE_I16  0x0114
#define RVL_PRIMITIVE_I32  0x0115
#define RVL_PRIMITIVE_I64  0x0116
#define RVL_PRIMITIVE_I128 0x0117
#define RVL_PRIMITIVE_F8   0x0123
#define RVL_PRIMITIVE_F16  0x0124
#define RVL_PRIMITIVE_F32  0x0125
#define RVL_PRIMITIVE_F64  0x0126
#define RVL_PRIMITIVE_F128 0x0127

#define RVL_PRIMITIVE_VEC2U8   0x0203
#define RVL_PRIMITIVE_VEC2U16  0x0204
#define RVL_PRIMITIVE_VEC2U32  0x0205
#define RVL_PRIMITIVE_VEC2U64  0x0206
#define RVL_PRIMITIVE_VEC2U128 0x0207
#define RVL_PRIMITIVE_VEC2I8   0x0213
#define RVL_PRIMITIVE_VEC2I16  0x0214
#define RVL_PRIMITIVE_VEC2I32  0x0215
#define RVL_PRIMITIVE_VEC2I64  0x0216
#define RVL_PRIMITIVE_VEC2I128 0x0217
#define RVL_PRIMITIVE_VEC2F8   0x0223
#define RVL_PRIMITIVE_VEC2F16  0x0224
#define RVL_PRIMITIVE_VEC2F32  0x0225
#define RVL_PRIMITIVE_VEC2F64  0x0226
#define RVL_PRIMITIVE_VEC2F128 0x0227

#define RVL_PRIMITIVE_VEC3U8   0x0303
#define RVL_PRIMITIVE_VEC3U16  0x0304
#define RVL_PRIMITIVE_VEC3U32  0x0305
#define RVL_PRIMITIVE_VEC3U64  0x0306
#define RVL_PRIMITIVE_VEC3U128 0x0307
#define RVL_PRIMITIVE_VEC3I8   0x0313
#define RVL_PRIMITIVE_VEC3I16  0x0314
#define RVL_PRIMITIVE_VEC3I32  0x0315
#define RVL_PRIMITIVE_VEC3I64  0x0316
#define RVL_PRIMITIVE_VEC3I128 0x0317
#define RVL_PRIMITIVE_VEC3F8   0x0323
#define RVL_PRIMITIVE_VEC3F16  0x0324
#define RVL_PRIMITIVE_VEC3F32  0x0325
#define RVL_PRIMITIVE_VEC3F64  0x0326
#define RVL_PRIMITIVE_VEC3F128 0x0327

#define RVL_PRIMITIVE_VEC4U8   0X0403
#define RVL_PRIMITIVE_VEC4U16  0X0404
#define RVL_PRIMITIVE_VEC4U32  0X0405
#define RVL_PRIMITIVE_VEC4U64  0X0406
#define RVL_PRIMITIVE_VEC4U128 0X0407
#define RVL_PRIMITIVE_VEC4I8   0X0413
#define RVL_PRIMITIVE_VEC4I16  0X0414
#define RVL_PRIMITIVE_VEC4I32  0X0415
#define RVL_PRIMITIVE_VEC4I64  0X0416
#define RVL_PRIMITIVE_VEC4I128 0X0417
#define RVL_PRIMITIVE_VEC4F8   0X0423
#define RVL_PRIMITIVE_VEC4F16  0X0424
#define RVL_PRIMITIVE_VEC4F32  0X0425
#define RVL_PRIMITIVE_VEC4F64  0X0426
#define RVL_PRIMITIVE_VEC4F128 0X0427

#define RVL_ENDIAN_BIG    0
#define RVL_ENDIAN_LITTLE 1

#define RVL_GRID_REGULAR     0
#define RVL_GRID_RECTILINEAR 1

#define RVL_UNIT_NA        0X00
#define RVL_UNIT_METER     0X01
#define RVL_UNIT_MILIMETER 0X02
#define RVL_UNIT_KILOMETER 0X03

#define RVL_COMPRESSION_LZMA2 0x00
#define RVL_COMPRESSION_LZ4   0x01

#define RVL_TEXT_TITLE         0x0D01
#define RVL_TEXT_DESCRIPTION   0x0D02
#define RVL_TEXT_AUTHOR        0x0D03
#define RVL_TEXT_COPYRIGHT     0x0D04
#define RVL_TEXT_LICENSE       0x0D05
#define RVL_TEXT_SOURCE        0x0D06
#define RVL_TEXT_CREATION_TIME 0x0D07

// Prevent name-mangling for librvl functions from C++ compiler.
#ifdef __cplusplus
extern "C"
{
#endif

#if defined(__GNUC__) && (__GNUC__ >= 4)
#  define RVLLIB_API __attribute__ ((visibility ("default")))
#elif defined(_WIN32)
#  ifdef RVLLIB_EXPORTS
#    define RVLLIB_API __declspec(dllexport)
#  else
#    define RVLLIB_API __declspec(dllimport)
#  endif
#else
#  define RVLLIB_API
#endif

#ifdef __cplusplus
#  define RVL_DEPRECATED(reason) [[deprecated(reason)]]
#elif defined(__GNUC__) || defined(__clang__)
#  define RVL_DEPRECATED(reason)  __attribute__ ((deprecated (reason)))
#elif defined(_WIN32)
#  define RVL_DEPRECATED(reason) __declspec(deprecated(reason))
#endif

RVLLIB_API RVL *rvl_create_writer (void);
RVLLIB_API RVL *rvl_create_reader (void);
RVLLIB_API void rvl_destroy (RVL **self);

RVLLIB_API void rvl_set_file (RVL *self, const char *filename);
RVLLIB_API void rvl_set_io (RVL *self, FILE *stream);

// Write the entire rvl file
RVLLIB_API void rvl_write_rvl (RVL *self);

// Read the entire rvl file
RVLLIB_API void rvl_read_rvl (RVL *self);

// Read the information without the actual volumetric data
// Use rvl_get_* to retrieve information.
RVLLIB_API void rvl_read_info (RVL *self);

// Read the entire volumetric data into the buffer. The buffer allocation and
// deallocation is managed by the user.
RVLLIB_API void rvl_read_voxels_to (RVL *self, void *buffer);

/* VFMT chunk functions */
RVLLIB_API void rvl_set_volumetric_format (RVL *self, int nx, int ny, int nz,
                                           RVLenum primitive, RVLenum endian);
RVLLIB_API void rvl_get_volumetric_format (RVL *self, int *nx, int *ny, int *nz,
                                           RVLenum *primitive, RVLenum *endian);

RVLLIB_API void    rvl_set_compression (RVL *self, RVLenum compression);
RVLLIB_API RVLenum rvl_get_compression (RVL *self);

// Get the byte size of the primitive type.
RVLLIB_API unsigned int rvl_sizeof (RVLenum primitive);


/* GRID chunk functions */
RVLLIB_API void rvl_set_regular_grid (RVL *self, float dx, float dy, float dz);
RVLLIB_API void rvl_set_rectilinear_grid (RVL *self, int ndx, int ndy, int ndz,
                                          float *dx, float *dy, float *dz);
RVLLIB_API void rvl_set_grid_unit (RVL *self, RVLenum unit);
RVLLIB_API void rvl_set_grid_origin (RVL *self, float x0, float y0, float z0);

RVLLIB_API RVLenum rvl_get_grid_type (RVL *self);
RVLLIB_API RVLenum rvl_get_grid_unit (RVL *self);

RVLLIB_API void rvl_get_grid_origin (RVL *self, float *x, float *y, float *z);
RVLLIB_API void rvl_get_voxel_dims (RVL *self, float *dx, float *dy, float *dz);
RVLLIB_API void rvl_get_voxel_dims_v (RVL *self, int *ndx, int *ndy, int *ndz,
                                      const float **dx, const float **dy,
                                      const float **dz);

/* DATA chunk functions */

// Set the voxel data to be written to file stream by RVL writer instance. The
// instance does not own the pointer; users should allocate the memory before
// writing and deallocate the memory after writing.
RVLLIB_API void rvl_set_voxels (RVL *self, const void *voxels);

// Get the voxel data from RVL reader instance. The instance owns the
// pointer, and users should not free the memory themselves.
RVLLIB_API void* rvl_get_voxels (RVL *self);

// Get the pointer to the voxel by x, y, and z indices.
RVLLIB_API void* rvl_get_voxel_at (RVL *self, int x, int y, int z);

/* TEXT chunk functions */
RVLLIB_API void rvl_set_text (RVL *self, RVLenum tag, const char *value);
RVLLIB_API const char* rvl_get_text_value (RVL *self, RVLenum tag);

#ifdef __cplusplus
}
#endif

#endif


