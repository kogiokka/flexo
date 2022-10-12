#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/rvl.h"

static void init_info (RVL *rvl);

void
rvl_test_write_INFO ()
{
  RVL *rvl = rvl_create_writer ("test_INFO.rvl");
  init_info (rvl);
  rvl_write_rvl (rvl);

  rvl_destroy (&rvl);
}

void
rvl_test_read_INFO ()
{
  RVL *rvl = rvl_create_reader ("test_INFO.rvl");

  rvl_read_rvl (rvl);

  RVLGridType gridType = rvl_get_grid_type (rvl);
  RVLGridUnit unit = rvl_get_grid_unit (rvl);
  RVLValueFormat format = rvl_get_value_format (rvl);
  RVLValueBitDepth bitDepth = rvl_get_value_bit_depth (rvl);
  RVLValueDimen dimen = rvl_get_value_dimension (rvl);
  RVLEndian endian = rvl_get_endian (rvl);
  int x, y, z;
  float vx, vy, vz;
  float px, py, pz;
  rvl_get_resolution (rvl, &x, &y, &z);
  rvl_get_voxel_size (rvl, &vx, &vy, &vz);
  rvl_get_position (rvl, &px, &py, &pz);

  fprintf (stdout, "Width: %d, Length: %d, Height: %d\n", x, y, z);
  fprintf (stdout, "Grid - type: %d, unit: %d\n", gridType, unit);
  fprintf (stdout, "Data Form - format: %d, bits: %d, dimensions: %d\n",
           format, bitDepth, dimen);
  fprintf (stdout, "Endian - %d\n", endian);
  fprintf (stdout, "Voxel Size - x: %.3f, y: %.3f, z: %.3f\n", vx, vy, vz);
  fprintf (stdout, "Position - x: %.3f, y: %.3f, z: %.3f\n", px, py, pz);

  if (unit != RVLGridUnit_NA)
    {
      exit (EXIT_FAILURE);
    }

  rvl_destroy (&rvl);
}

void
rvl_test_write_DATA ()
{
  RVL *rvl = rvl_create_writer ("test_DATA.rvl");
  init_info (rvl);

  RVLByte *buffer;
  RVLSize size;

  rvl_alloc_data_buffer (rvl, &buffer, &size);
  memset (buffer, 'A', size);
  rvl_set_data_buffer (rvl, buffer, size);
  rvl_write_rvl (rvl);
  rvl_dealloc_data_buffer (rvl, &buffer);

  rvl_destroy (&rvl);
}

void
rvl_test_read_DATA ()
{
  RVL *rvl = rvl_create_reader ("test_DATA.rvl");

  RVLByte *buffer;
  RVLSize size;

  rvl_read_rvl (rvl);
  rvl_get_data_buffer (rvl, &buffer, &size);

  fwrite (buffer, 1, size, stdout);

  rvl_destroy (&rvl);
}

void
rvl_test_write_TEXT ()
{
  RVL *rvl = rvl_create_writer ("test_TEXT.rvl");
  init_info (rvl);

  int numText = 2;
  RVLText *textArr = rvl_text_create_array (numText);
  rvl_text_set (textArr, 0, "Title", "librvl");
  rvl_text_set (textArr, 1, "Description",
                "The Regular VoLumetric format reference library");
  rvl_set_text (rvl, &textArr, numText);
  if (textArr != NULL)
    {
      exit (EXIT_FAILURE);
    }

  rvl_write_rvl (rvl);

  rvl_destroy (&rvl);
}

void
rvl_test_read_TEXT ()
{
  RVL *rvl = rvl_create_reader ("test_TEXT.rvl");
  rvl_read_rvl (rvl);
  RVLText *textArr;
  int numText;
  rvl_get_text (rvl, &textArr, &numText);

  const char *key = NULL;
  const char *value = NULL;
  for (int i = 0; i < numText; i++)
    {
      rvl_text_get (textArr, i, &key, &value);
      fprintf (stdout, "%s: %s\n", key, value);
    }
  rvl_destroy (&rvl);
}

void
rvl_test_write ()
{
  RVL *rvl = rvl_create_writer ("test.rvl");
  init_info (rvl);

  int numText = 2;
  RVLText *textArr = rvl_text_create_array (numText);
  rvl_text_set (textArr, 0, "Title", "librvl");
  rvl_text_set (textArr, 1, "Description",
                "The Regular VoLumetric format reference library");
  rvl_set_text (rvl, &textArr, numText);

  RVLByte *buffer;
  RVLSize size;
  rvl_alloc_data_buffer (rvl, &buffer, &size);
  memset (buffer, 'A', size);
  rvl_set_data_buffer (rvl, buffer, size);

  rvl_write_rvl (rvl);
  rvl_dealloc_data_buffer (rvl, &buffer);
  rvl_destroy (&rvl);
}

void
rvl_test_read_parts ()
{
  RVL *rvl = rvl_create_reader ("test.rvl");

  // Read INFO and TEXT
  RVLText *textArr;
  int numText;
  const char *key = NULL;
  const char *value = NULL;

  rvl_read_info (rvl);
  rvl_get_text (rvl, &textArr, &numText);
  for (int i = 0; i < numText; i++)
    {
      rvl_text_get (textArr, i, &key, &value);
      fprintf (stdout, "%s: %s\n", key, value);
    }

  RVLByte *buffer;
  RVLSize size;
  rvl_alloc_data_buffer (rvl, &buffer, &size);
  rvl_read_data_buffer (rvl, &buffer);
  fwrite (buffer, 1, size, stdout);
  rvl_dealloc_data_buffer (rvl, &buffer);

  rvl_destroy (&rvl);
}

void
init_info (RVL *rvl)
{
  rvl_set_grid_type (rvl, RVLGridType_Cartesian);
  rvl_set_grid_unit (rvl, RVLGridUnit_NA);
  rvl_set_value_format (rvl, RVLValueFormat_Unsigned);
  rvl_set_value_bit_depth (rvl, RVLValueBitDepth_8);
  rvl_set_value_dimension (rvl, RVLValueDimen_Scalar);
  rvl_set_endian (rvl, RVLEndian_Little);
  rvl_set_resolution (rvl, 20, 20, 20);
  rvl_set_voxel_size (rvl, 1.0f, 1.0f, 1.0f);
  rvl_set_position (rvl, 0.0f, 0.0f, 0.0f);
}
