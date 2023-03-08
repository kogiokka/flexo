#include <rvl.h>
#include <stdbool.h>
#include <stdlib.h>

static const char VOXELS[1] = { 'A' };

bool
verify (const char *filename)
{
  RVLenum prm;
  RVLenum endian;
  int     nx, ny, nz;

  RVL *rvl = rvl_create_reader ();
  rvl_set_file (rvl, filename);
  rvl_read_rvl (rvl);
  rvl_get_volumetric_format (rvl, &nx, &ny, &nz, &prm, &endian);

  char voxel = *(char *)rvl_get_voxels (rvl);
  rvl_destroy (&rvl);

  if (voxel != VOXELS[0])
    {
      return false;
    }

  return true;
}

int
main ()
{
  RVL *rvl = rvl_create_writer ();
  rvl_set_volumetric_format (rvl, 1, 1, 1, RVL_PRIMITIVE_I8,
                             RVL_ENDIAN_LITTLE);
  rvl_set_regular_grid (rvl, 0.1f, 0.1f, 0.1f);
  rvl_set_voxels (rvl, VOXELS);

  rvl_set_compression (rvl, RVL_COMPRESSION_LZMA2);
  rvl_set_file (rvl, "tiny-lzma2.rvl");
  rvl_write_rvl (rvl);

  rvl_set_compression (rvl, RVL_COMPRESSION_LZ4);
  rvl_set_file (rvl, "tiny-lz4.rvl");
  rvl_write_rvl (rvl);

  rvl_destroy (&rvl);

  verify ("tiny-lzma2.rvl");
  verify ("tiny-lz4.rvl");
  return 0;
}
