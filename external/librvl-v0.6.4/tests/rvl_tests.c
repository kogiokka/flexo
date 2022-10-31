#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/rvl.h"

static void init_regular_grid (RVL *rvl);
static void init_rectilinear_grid (RVL *rvl);
static void print_data_buffer (int x, int y, int z, const void *buffer,
                               int primSize);

void
rvl_test_write_regular_grid ()
{
  RVL *rvl = rvl_create_writer ();
  rvl_set_file (rvl, "test_regular.rvl");

  // VFMT/GRID chunk
  init_regular_grid (rvl);

  // DATA chunk
  int   size   = rvl_get_data_nbytes (rvl);
  void *buffer = (void *)malloc (size);
  memset (buffer, 'A', size);
  rvl_set_data_buffer (rvl, size, buffer);

  // TEXT chunk
  rvl_set_text (rvl, RVL_TAG_TITLE, "librvl");
  rvl_set_text (rvl, RVL_TAG_DESCRIPTION,
                "The Regular VoLumetric format reference library");

  // Write to file
  rvl_write_rvl (rvl);

  free (buffer);
  rvl_destroy (&rvl);
}

void
rvl_test_read_regular_grid ()
{
  RVL *rvl = rvl_create_reader ();
  rvl_set_file (rvl, "test_regular.rvl");

  // Read from file
  rvl_read_rvl (rvl);

  // VFMT/GRID chunk
  RVLenum gridType = rvl_get_grid_type (rvl);
  RVLenum unit     = rvl_get_grid_unit (rvl);
  int     x, y, z;
  float   dx, dy, dz;
  float   px, py, pz;
  RVLenum primitive, endian;
  rvl_get_volumetric_format (rvl, &x, &y, &z, &primitive, &endian);
  rvl_get_voxel_dims (rvl, &dx, &dy, &dz);
  rvl_get_grid_position (rvl, &px, &py, &pz);

  RVLenum compress = rvl_get_compression (rvl);

  char sep[81];
  sep[80] = '\0';
  memset (sep, '-', 80);
  fprintf (stdout, "%s\n", sep);
  fprintf (stdout, "Width: %d, Length: %d, Height: %d\n", x, y, z);
  fprintf (stdout, "Grid - type: %d, unit: %d\n", gridType, unit);
  fprintf (stdout, "Data format: 0x%.4x\n", primitive);
  fprintf (stdout, "Endian - %d\n", endian);
  fprintf (stdout, "Compression method - %d\n", compress);
  fprintf (stdout, "Voxel Dim - x: %.3f, y: %.3f, z: %.3f\n", dx, dy, dz);
  fprintf (stdout, "Position - x: %.3f, y: %.3f, z: %.3f\n", px, py, pz);
  fprintf (stdout, "%s\n", sep);

  if (unit != RVL_UNIT_NA)
    {
      exit (EXIT_FAILURE);
    }

  // DATA chunk
  const void *buffer;
  rvl_get_data_buffer (rvl, &buffer);
  print_data_buffer (x, y, z, buffer, rvl_get_primitive_nbytes (rvl));

  // TEXT chunk
  const char *title;
  const char *descr;
  rvl_get_text (rvl, RVL_TAG_TITLE, &title);
  rvl_get_text (rvl, RVL_TAG_DESCRIPTION, &descr);
  fprintf (stdout, "Title: %s\n", title);
  fprintf (stdout, "Description: %s\n", descr);

  rvl_destroy (&rvl);
}

void
rvl_test_write_rectilinear_grid ()
{
  RVL *rvl = rvl_create_writer ();
  rvl_set_file (rvl, "test_rectilinear.rvl");

  // VFMT/GRID chunk
  init_rectilinear_grid (rvl);

  // DATA chunk
  int   size   = rvl_get_data_nbytes (rvl);
  void *buffer = (void *)malloc (size);
  memset (buffer, 'A', size);
  rvl_set_data_buffer (rvl, size, buffer);

  // TEXT chunk
  rvl_set_text (rvl, RVL_TAG_TITLE, "librvl");
  rvl_set_text (rvl, RVL_TAG_DESCRIPTION,
                "The Regular VoLumetric format reference library");

  // Write to file
  rvl_write_rvl (rvl);

  free (buffer);
  rvl_destroy (&rvl);
}

void
rvl_test_read_rectilinear_grid ()
{
  RVL *rvl = rvl_create_reader ();
  rvl_set_file (rvl, "test_rectilinear.rvl");

  // Read from file
  rvl_read_rvl (rvl);

  // VFMT/GRID chunk
  RVLenum gridType = rvl_get_grid_type (rvl);
  RVLenum unit     = rvl_get_grid_unit (rvl);

  int          x, y, z;
  int          ndx, ndy, ndz;
  const float *dx, *dy, *dz;
  float        px, py, pz;
  RVLenum      primitive, endian;
  rvl_get_volumetric_format (rvl, &x, &y, &z, &primitive, &endian);
  rvl_get_voxel_dims_v (rvl, &ndx, &ndy, &ndz, &dx, &dy, &dz);
  rvl_get_grid_position (rvl, &px, &py, &pz);

  RVLenum compress = rvl_get_compression (rvl);

  char sep[81];
  sep[80] = '\0';
  memset (sep, '-', 80);
  fprintf (stdout, "%s\n", sep);
  fprintf (stdout, "Width: %d, Length: %d, Height: %d\n", x, y, z);
  fprintf (stdout, "Grid - type: %d, unit: %d\n", gridType, unit);
  fprintf (stdout, "Data format: 0x%.4x\n", primitive);
  fprintf (stdout, "Endian - %d\n", endian);
  fprintf (stdout, "Compresson method - %d\n", compress);
  fprintf (stdout, "Voxel Dim -\n");

  if (ndx <= 0 || ndy <= 0 || ndx <= 0)
    {
      exit (EXIT_FAILURE);
    }

  for (int i = 0; i < ndx; i++)
    {
      fprintf (stdout, "    x%d: %.3f\n", i, dx[i]);
    }
  for (int i = 0; i < ndy; i++)
    {
      fprintf (stdout, "    y%d: %.3f\n", i, dy[i]);
    }
  for (int i = 0; i < ndz; i++)
    {
      fprintf (stdout, "    z%d: %.3f\n", i, dz[i]);
    }
  fprintf (stdout, "Position - x: %.3f, y: %.3f, z: %.3f\n", px, py, pz);
  fprintf (stdout, "%s\n", sep);

  // DATA chunk
  const void *buffer;
  rvl_get_data_buffer (rvl, &buffer);
  print_data_buffer (x, y, z, buffer, rvl_get_primitive_nbytes (rvl));

  // TEXT chunk
  const char *title;
  const char *descr;
  rvl_get_text (rvl, RVL_TAG_TITLE, &title);
  rvl_get_text (rvl, RVL_TAG_DESCRIPTION, &descr);
  fprintf (stdout, "Title: %s\n", title);
  fprintf (stdout, "Description: %s\n", descr);

  rvl_destroy (&rvl);
}

void
rvl_test_partially_read ()
{
  RVL *rvl = rvl_create_reader ();
  rvl_set_file (rvl, "test_regular.rvl");

  // Read VFMT, GRID and TEXT
  rvl_read_info (rvl);

  int   x, y, z;
  float dx, dy, dz;
  float px, py, pz;

  RVLenum gridType = rvl_get_grid_type (rvl);
  RVLenum unit     = rvl_get_grid_unit (rvl);
  RVLenum primitive, endian;

  rvl_get_volumetric_format (rvl, &x, &y, &z, &primitive, &endian);
  rvl_get_voxel_dims (rvl, &dx, &dy, &dz);
  rvl_get_grid_position (rvl, &px, &py, &pz);

  const char *title;
  const char *descr;
  rvl_get_text (rvl, RVL_TAG_TITLE, &title);
  rvl_get_text (rvl, RVL_TAG_DESCRIPTION, &descr);
  fprintf (stdout, "Title: %s\n", title);
  fprintf (stdout, "Description: %s\n", descr);

  RVLenum compress = rvl_get_compression (rvl);

  // Print RVL information
  char sep[81];
  sep[80] = '\0';
  memset (sep, '-', 80);
  fprintf (stdout, "%s\n", sep);
  fprintf (stdout, "Width: %d, Length: %d, Height: %d\n", x, y, z);
  fprintf (stdout, "Grid - type: %d, unit: %d\n", gridType, unit);
  fprintf (stdout, "Data format: 0x%.4x\n", primitive);
  fprintf (stdout, "Endian - %d\n", endian);
  fprintf (stdout, "Compression - %d\n", compress);
  fprintf (stdout, "Voxel Dim - x: %.3f, y: %.3f, z: %.3f\n", dx, dy, dz);

  // Read DATA
  void *buffer = (void *)malloc (rvl_get_data_nbytes (rvl));
  rvl_read_data_buffer (rvl, &buffer);
  print_data_buffer (x, y, z, buffer, rvl_get_primitive_nbytes (rvl));
  free (buffer);

  rvl_destroy (&rvl);
}

void
rvl_test_uninitialized_rvl ()
{
  RVL *rvl = NULL;
  rvl_destroy (&rvl);

  rvl = rvl_create_reader ();
  rvl_destroy (&rvl);

  rvl = rvl_create_writer ();
  rvl_destroy (&rvl);
}

void
rvl_test_io ()
{
  RVL *rvl = rvl_create_writer ();
  init_regular_grid (rvl);

  int   size   = rvl_get_data_nbytes (rvl);
  void *buffer = (void *)malloc (size);
  memset (buffer, 'O', size);
  rvl_set_data_buffer (rvl, size, buffer);

  rvl_set_file (rvl, "rvl_test_io.rvl");
  rvl_write_rvl (rvl);

  rvl_set_io (rvl, stdout);
  rvl_write_rvl (rvl);

  rvl_set_io (rvl, stderr);
  rvl_set_io (rvl, stdout);
  rvl_write_rvl (rvl);

  rvl_set_file (rvl, "rvl_test_io_2.rvl");
  rvl_set_file (rvl, "rvl_test_io_3.rvl");
  rvl_write_rvl (rvl);

  free (buffer);
  rvl_destroy (&rvl);
}

void
init_regular_grid (RVL *rvl)
{
  rvl_set_volumetric_format (rvl, 2, 2, 2, RVL_PRIMITIVE_VEC2U8,
                             RVL_ENDIAN_LITTLE);

  rvl_set_regular_grid (rvl, 0.1f, 0.2f, 0.3f);
  rvl_set_grid_origin (rvl, 3.0f, 2.0f, 1.0f);
  rvl_set_grid_unit (rvl, RVL_UNIT_NA);
}

void
init_rectilinear_grid (RVL *rvl)
{
  rvl_set_volumetric_format (rvl, 6, 6, 3, RVL_PRIMITIVE_F8,
                             RVL_ENDIAN_LITTLE);

  float dx[6];
  float dy[6];
  float dz[3];

  for (int i = 0; i < 6; i++)
    {
      dx[i] = i * 2.0f;
      dy[i] = i * 0.4f;
      if (i < 3)
        {
          dz[i] = i * 3.0f;
        }
    }

  rvl_set_rectilinear_grid (rvl, 6, 6, 3, dx, dy, dz);
  rvl_set_grid_origin (rvl, 1.0f, 2.0f, 3.0f);
  rvl_set_grid_unit (rvl, RVL_UNIT_METER);
}

void
print_data_buffer (int x, int y, int z, const void *buffer, int primSize)
{
  const char *data = (char *)buffer;
  fprintf (stdout, "Prim size: %d\n", primSize);
  for (int k = 0; k < z; k++)
    {
      for (int j = 0; j < y; j++)
        {
          for (int i = 0; i < x - 1; i++)
            {
              fwrite (&data[i + j * x + k * x * y], 1, primSize, stdout);
              fprintf (stdout, "|");
            }
          fwrite (&data[x - 1 + j * x + k * x * y], 1, primSize, stdout);
          fprintf (stdout, "\n");
        }
      fprintf (stdout, "\n");
    }
}

