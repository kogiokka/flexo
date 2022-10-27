#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rvl.h>

static void write_rvl (RVL *rvl);
static void read_rvl (RVL *rvl);
static void print_data_buffer (int x, int y, int z, const void *buffer);

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
  int nx = 9;
  int ny = 16;
  int nz = 25;

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
              float dx   = (k - center[0]);
              float dy   = (j - center[1]);
              float dz   = (i - center[2]);
              float dist = sqrt (dx * dx + dy * dy + dz * dz);

              buffer[idx] = dist;
            }
        }
    }

  rvl_set_regular_grid (rvl, 0.1f, 0.2f, 0.1f);
  rvl_set_volumetric_format (rvl, nx, ny, nz, RVL_PRIMITIVE_F32,
                             RVL_ENDIAN_LITTLE);
  rvl_set_data_buffer (rvl, size, buffer);

  rvl_set_text (rvl, RVL_TAG_TITLE, "Sphere");
  rvl_set_text (rvl, RVL_TAG_DESCRIPTION,
                "Distances from the center of the volumetric data.");
  rvl_set_text (rvl, RVL_TAG_AUTHOR, "John Doe");
  rvl_set_text (rvl, RVL_TAG_COPYRIGHT,
                "Copyright (c) 2022 by John Doe, All Rights Reserved.");
  rvl_set_text (rvl, RVL_TAG_LICENSE, "CC BY-SA 4.0");
  rvl_set_text (rvl, RVL_TAG_CREATION_TIME, "2022-10-26T05:13:44+08:00");

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
  int     x, y, z;
  float   dx, dy, dz;
  float   x0, y0, z0;
  rvl_get_volumetric_format (rvl, &x, &y, &z, &primitive, &endian);
  rvl_get_voxel_dims (rvl, &dx, &dy, &dz);
  rvl_get_grid_position (rvl, &x0, &y0, &z0);

  char sep[81];
  memset (sep, '-', 80);
  sep[80] = '\0';
  fprintf (stdout, "%s\n", sep);
  fprintf (stdout, "Width (x): %d\n", x);
  fprintf (stdout, "Length (y): %d\n", y);
  fprintf (stdout, "Height (z): %d\n", z);
  fprintf (stdout, "Grid type: 0x%.4X\n", gridType);
  fprintf (stdout, "Data primitive: 0x%.4X\n", primitive);
  fprintf (stdout, "Endianness: %d\n", endian);
  fprintf (stdout, "Voxel dimensions: (%.3f, %.3f, %.3f)\n", dx, dy, dz);
  fprintf (stdout, "Grid position of the origin: (%.3f, %.3f, %.3f)\n", x0, y0, z0);
  fprintf (stdout, "Grid unit: 0x%.4X\n", unit);
  fprintf (stdout, "%s\n", sep);

  const void *buffer;
  rvl_get_data_buffer (rvl, &buffer);
  print_data_buffer (x, y, z, buffer);

  const char *title, *descr, *author, *copyright, *license, *source, *ctime;
  rvl_get_text (rvl, RVL_TAG_TITLE, &title);
  rvl_get_text (rvl, RVL_TAG_DESCRIPTION, &descr);
  rvl_get_text (rvl, RVL_TAG_AUTHOR, &author);
  rvl_get_text (rvl, RVL_TAG_COPYRIGHT, &copyright);
  rvl_get_text (rvl, RVL_TAG_LICENSE, &license);
  rvl_get_text (rvl, RVL_TAG_SOURCE, &source);
  rvl_get_text (rvl, RVL_TAG_CREATION_TIME, &ctime);

  fprintf (stdout, "Title: %s\n", title);
  fprintf (stdout, "Description: %s\n", descr);
  fprintf (stdout, "Author: %s\n", author);
  fprintf (stdout, "Copyright: %s\n", copyright);
  fprintf (stdout, "License: %s\n", license);
  fprintf (stdout, "Source: %s\n", source);
  fprintf (stdout, "Creation time: %s\n", ctime);
}

void
print_data_buffer (int x, int y, int z, const void *buffer)
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
