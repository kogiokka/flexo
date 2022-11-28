#include <rvl.h>
#include <stdlib.h>
#include <string.h>

#define NX 2
#define NY 2
#define NZ 2

int
main ()
{
  RVL    *rvl = rvl_create_writer ();
  RVLenum prm = RVL_PRIMITIVE_I8;

  rvl_set_volumetric_format (rvl, NX, NY, NZ, prm, RVL_ENDIAN_BIG);
  rvl_set_regular_grid (rvl, 1.0f, 1.0f, 1.0f);
  rvl_set_text (rvl, RVL_TEXT_AUTHOR, "John Doe");

  int   size = NX * NY * NZ * rvl_sizeof (prm);
  void *data = (void *)malloc (size);
  memset (data, 'x', size);
  rvl_set_voxels (rvl, data);

  rvl_set_file (rvl, "io.rvl");
  rvl_write_rvl (rvl);

  rvl_set_io (rvl, stdout);
  rvl_write_rvl (rvl);

  rvl_set_io (rvl, stderr);
  rvl_set_io (rvl, stdout);
  rvl_set_io (rvl, stderr);
  rvl_write_rvl (rvl);

  rvl_set_file (rvl, "io_2.rvl");
  rvl_set_file (rvl, "io_3.rvl");
  rvl_write_rvl (rvl);

  free (data);
  rvl_destroy (&rvl);
}
