#include <rvl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NX 33
#define NY 55
#define NZ 72

static float VOXELS[NX * NY * NZ];

void
genVoxels ()
{
  srand (time (NULL));

  for (int i = 0; i < NZ; i++)
    {
      for (int j = 0; j < NY; j++)
        {
          for (int k = 0; k < NX; k++)
            {
              int index     = k + j * NX + i * NX * NY;
              VOXELS[index] = ((float)rand () / (float)RAND_MAX);
            }
        }
    }
}

int
main ()
{
  RVL *rvl = NULL;

  rvl = rvl_create_writer ();
  rvl_set_volumetric_format (rvl, NX, NY, NZ, RVL_PRIMITIVE_F32,
                             RVL_ENDIAN_LITTLE);
  rvl_set_regular_grid (rvl, 1.0f, 1.0f, 1.0f);
  rvl_set_compression (rvl, RVL_COMPRESSION_LZ4);

  rvl_set_voxels (rvl, VOXELS);

  rvl_set_file (rvl, "lz4.rvl");
  rvl_write_rvl (rvl);

  rvl_destroy (&rvl);

  rvl = rvl_create_reader ();
  rvl_set_file (rvl, "lz4.rvl");
  rvl_read_rvl (rvl);
  float *data = (float *)rvl_get_voxels (rvl);

  for (int i = 0; i < NZ; i++)
    {
      for (int j = 0; j < NY; j++)
        {
          for (int k = 0; k < NX; k++)
            {
              int index = k + j * NX + i * NX * NY;
              if (VOXELS[index] != data[index])
                {
                  exit (EXIT_FAILURE);
                }
            }
        }
    }

  rvl_destroy (&rvl);
}
