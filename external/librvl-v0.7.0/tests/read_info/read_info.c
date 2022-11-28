#include <rvl.h>
#include <stdlib.h>
#include <string.h>

static float       VX_DIM[3] = { 0.1f, 0.2f, 0.3f };
static int         RES[3]    = { 2, 2, 2 };
static RVLenum     PRM       = RVL_PRIMITIVE_U8;
static RVLenum     ENDIAN    = RVL_ENDIAN_LITTLE;
static const char *AUTHOR    = "John Doe";

int
main ()
{
  RVL *rvl = rvl_create_writer ();
  rvl_set_volumetric_format (rvl, RES[0], RES[1], RES[2], PRM, ENDIAN);
  rvl_set_regular_grid (rvl, VX_DIM[0], VX_DIM[1], VX_DIM[2]);
  rvl_set_text (rvl, RVL_TEXT_AUTHOR, "John Doe");

  float *data = (float *)malloc (RES[0] * RES[1] * RES[2]);
  rvl_set_voxels (rvl, data);

  rvl_set_file (rvl, "read_info.rvl");
  rvl_write_rvl (rvl);

  free (data);
  rvl_destroy (&rvl);

  RVLenum prm;
  RVLenum endian;
  int     nx, ny, nz;

  rvl = rvl_create_reader ();
  rvl_set_file (rvl, "read_info.rvl");
  rvl_read_info (rvl);
  rvl_get_volumetric_format (rvl, &nx, &ny, &nz, &prm, &endian);

  const char *author = rvl_get_text_value (rvl, RVL_TEXT_AUTHOR);

  if ((prm != PRM) || (endian != ENDIAN)
      || (nx != RES[0] || ny != RES[1] || nz != RES[2])
      || (strcmp (author, AUTHOR) != 0))
    {
      exit (EXIT_FAILURE);
    }

  rvl_destroy(&rvl);
}
