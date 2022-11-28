#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rvl.h>

static void write_rvl (RVL *rvl);
static void read_rvl (RVL *rvl);
static void print_voxel_data (int x, int y, int z, const void *buffer);

int
main ()
{
  RVL *rvl;

  rvl = rvl_create_writer ();
  rvl_set_file (rvl, "sphere.rvl");
  write_rvl (rvl);
  rvl_destroy (&rvl);

  rvl = rvl_create_reader ();
  rvl_set_file (rvl, "sphere.rvl");
  read_rvl (rvl);
  rvl_destroy (&rvl);

  return 0;
}

void
write_rvl (RVL *rvl)
{
  int nx = 11;
  int ny = 25;
  int nz = 9;

  unsigned int size      = nx * ny * nz * sizeof (int);
  float       *buffer    = (float *)malloc (size);
  float        center[3] = { nx * 0.5f, ny * 0.5f, nz * 0.5f };

  for (int i = 0; i < nz; i++)
    {
      for (int j = 0; j < ny; j++)
        {
          for (int k = 0; k < nx; k++)
            {
              int   idx  = k + j * nx + i * nx * ny;
              float dx   = (k + 0.5f - center[0]);
              float dy   = (j + 0.5f - center[1]);
              float dz   = (i + 0.5f - center[2]);
              float dist = sqrt (dx * dx + dy * dy + dz * dz);

              buffer[idx] = dist;
            }
        }
    }

  rvl_set_regular_grid (rvl, 0.1f, 0.2f, 0.1f);
  rvl_set_volumetric_format (rvl, nx, ny, nz, RVL_PRIMITIVE_F32,
                             RVL_ENDIAN_LITTLE);
  rvl_set_voxels (rvl, buffer);

  rvl_set_text (rvl, RVL_TEXT_TITLE, "Sphere");
  rvl_set_text (rvl, RVL_TEXT_DESCRIPTION,
                "Distances from the center of the volumetric data.");
  rvl_set_text (rvl, RVL_TEXT_AUTHOR, "John Doe");
  rvl_set_text (rvl, RVL_TEXT_COPYRIGHT,
                "Copyright (c) 2022 by John Doe, All Rights Reserved.");
  rvl_set_text (rvl, RVL_TEXT_LICENSE, "CC BY-SA 4.0");
  rvl_set_text (rvl, RVL_TEXT_CREATION_TIME, "2022-10-26T05:13:44+08:00");

  // Write to file
  rvl_write_rvl (rvl);

  free (buffer);
}

void
read_rvl (RVL *rvl)
{
  // Read from file
  rvl_read_rvl (rvl);

  RVLenum gridType = rvl_get_grid_type (rvl);
  RVLenum unit     = rvl_get_grid_unit (rvl);
  RVLenum primitive, endian;
  int     nx, ny, nz;
  float   dx, dy, dz;
  float   x0, y0, z0;
  rvl_get_volumetric_format (rvl, &nx, &ny, &nz, &primitive, &endian);
  rvl_get_voxel_dims (rvl, &dx, &dy, &dz);
  rvl_get_grid_origin (rvl, &x0, &y0, &z0);

  char sep[81];
  memset (sep, '-', 80);
  sep[80] = '\0';
  fprintf (stdout, "%s\n", sep);
  fprintf (stdout, "Width (x): %d\n", nx);
  fprintf (stdout, "Length (y): %d\n", ny);
  fprintf (stdout, "Height (z): %d\n", nz);
  fprintf (stdout, "Grid type: 0x%.4X\n", gridType);
  fprintf (stdout, "Data primitive: 0x%.4X\n", primitive);
  fprintf (stdout, "Endianness: %d\n", endian);
  fprintf (stdout, "Voxel dimensions: (%.3f, %.3f, %.3f)\n", dx, dy, dz);
  fprintf (stdout, "Grid position of the origin: (%.3f, %.3f, %.3f)\n", x0, y0,
           z0);
  fprintf (stdout, "Grid unit: 0x%.4X\n", unit);
  fprintf (stdout, "%s\n", sep);

  const void *buffer = rvl_get_voxels (rvl);
  print_voxel_data (nx, ny, nz, buffer);

  const char *title     = rvl_get_text_value (rvl, RVL_TEXT_TITLE);
  const char *descr     = rvl_get_text_value (rvl, RVL_TEXT_DESCRIPTION);
  const char *author    = rvl_get_text_value (rvl, RVL_TEXT_AUTHOR);
  const char *copyright = rvl_get_text_value (rvl, RVL_TEXT_COPYRIGHT);
  const char *license   = rvl_get_text_value (rvl, RVL_TEXT_LICENSE);
  const char *source    = rvl_get_text_value (rvl, RVL_TEXT_SOURCE);
  const char *ctime     = rvl_get_text_value (rvl, RVL_TEXT_CREATION_TIME);

  fprintf (stdout, "Title: %s\n", title);
  fprintf (stdout, "Description: %s\n", descr);
  fprintf (stdout, "Author: %s\n", author);
  fprintf (stdout, "Copyright: %s\n", copyright);
  fprintf (stdout, "License: %s\n", license);
  fprintf (stdout, "Source: %s\n", source);
  fprintf (stdout, "Creation time: %s\n", ctime);

  fprintf (stdout, "%s\n", sep);

  int    c[3] = { (nx + 1) / 2 - 1, (ny + 1) / 2 - 1, (nz + 1) / 2 - 1 };
  float *v    = rvl_get_voxel_at (rvl, c[0], c[1], c[2]);
  fprintf (stdout, "Voxel at (%d, %d, %d) has value: %6.3f.\n", c[0], c[1],
           c[2], *v);
}

void
print_voxel_data (int x, int y, int z, const void *buffer)
{
  const float *data = (float *)buffer;
  for (int k = 0; k < z; k++)
    {
      for (int j = 0; j < y; j++)
        {
          for (int i = 0; i < x - 1; i++)
            {
              fprintf (stdout, "%6.3f|", data[i + j * x + k * x * y]);
            }
          fprintf (stdout, "%6.3f", data[x - 1 + j * x + k * x * y]);
          fprintf (stdout, "\n");
        }
      fprintf (stdout, "\n");
    }
}
