#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rvl.h>

static void write_rvl (RVL *rvl);
static void read_rvl (RVL *rvl);
static void print_data_buffer (int x, int y, int z, RVLConstByte *buffer);

int
main ()
{
  RVL *rvlw = rvl_create_writer ("volumetric_data.rvl");
  write_rvl (rvlw);
  rvl_destroy (&rvlw);

  RVL *rvlr = rvl_create_reader ("volumetric_data.rvl");
  read_rvl (rvlr);
  rvl_destroy (&rvlr);
  return 0;
}

void
write_rvl (RVL *rvl)
{
  int rx = 20;
  int ry = 10;
  int rz = 20;

  RVLSize size   = rx * ry * rz * sizeof (int);
  int    *buffer = (int *)malloc (size);

  for (int i = 0; i < rz; i++)
    {
      for (int j = 0; j < ry; j++)
        {
          for (int k = 0; k < rx; k++)
            {
              buffer[k + j * rx + i * rx * ry] = i;
            }
        }
    }

  rvl_set_grid_type (rvl, RVLGridType_Regular);
  rvl_set_grid_unit (rvl, RVLGridUnit_NA);
  rvl_set_grid_position (rvl, 0.0f, 0.0f, 0.0f);
  rvl_set_resolution (rvl, rx, ry, rz);
  rvl_set_voxel_dims_3f (rvl, 0.1f, 0.2f, 0.1f);
  rvl_set_primitive (rvl, RVLPrimitive_i32);
  rvl_set_endian (rvl, RVLEndian_Little);

  rvl_set_data_buffer (rvl, size, (RVLByte *)buffer);

  int      numText = 2;
  RVLText *textArr = rvl_text_create_array (numText);
  rvl_text_set (textArr, 0, "Title", "librvl");
  rvl_text_set (textArr, 1, "Description",
                "The Regular VoLumetric format reference library");
  rvl_set_text (rvl, &textArr, numText);

  // Write to file
  rvl_write_rvl (rvl);

  free (buffer);
}

void
read_rvl (RVL *rvl)
{
  // Read from file
  rvl_read_rvl (rvl);

  RVLGridType  gridType = rvl_get_grid_type (rvl);
  RVLGridUnit  unit     = rvl_get_grid_unit (rvl);
  RVLPrimitive format   = rvl_get_primitive (rvl);
  RVLEndian    endian   = rvl_get_endian (rvl);
  int          x, y, z;
  float        dx, dy, dz;
  float        px, py, pz;
  rvl_get_resolution (rvl, &x, &y, &z);
  rvl_get_voxel_dims_3f (rvl, &dx, &dy, &dz);
  rvl_get_grid_position (rvl, &px, &py, &pz);

  char sep[81];
  memset (sep, '-', 80);
  sep[80] = '\0';
  fprintf (stdout, "%s\n", sep);
  fprintf (stdout, "Width: %d, Length: %d, Height: %d\n", x, y, z);
  fprintf (stdout, "Grid - type: %d, unit: %d\n", gridType, unit);
  fprintf (stdout, "Data format: 0x%.4x\n", format);
  fprintf (stdout, "Endian - %d\n", endian);
  fprintf (stdout, "Voxel Dim - x: %.3f, y: %.3f, z: %.3f\n", dx, dy, dz);
  fprintf (stdout, "Position - x: %.3f, y: %.3f, z: %.3f\n", px, py, pz);
  fprintf (stdout, "%s\n", sep);

  RVLByte *buffer;
  rvl_get_data_buffer (rvl, &buffer);
  print_data_buffer (x, y, z, buffer);

  RVLText *textArr;
  int      numText;
  rvl_get_text (rvl, &textArr, &numText);

  const char *key   = NULL;
  const char *value = NULL;
  for (int i = 0; i < numText; i++)
    {
      rvl_text_get (textArr, i, &key, &value);
      fprintf (stdout, "%s: %s\n", key, value);
    }
}

void
print_data_buffer (int x, int y, int z, RVLConstByte *buffer)
{
  const int *data = (int *)buffer;
  for (int k = 0; k < z; k++)
    {
      for (int j = 0; j < y; j++)
        {
          for (int i = 0; i < x - 1; i++)
            {
              fprintf (stdout, "%d|", data[i + j * x + k * x * y]);
            }
          fprintf (stdout, "%d", data[x - 1 + j * x + k * x * y]);
          fprintf (stdout, "\n");
        }
      fprintf (stdout, "\n");
    }
}
