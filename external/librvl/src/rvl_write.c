#include <stdlib.h>

#include <log.h>

#include "rvl.h"

#include "detail/rvl_write_p.h"

void
rvl_write_rvl (RVL *self)
{
  if (self->grid.vxDimBufSize <= 0)
    {
      log_fatal ("[librvl write] Missing voxel dimensions.");
      exit (EXIT_FAILURE);
    }

  if (self->data.size <= 0)
    {
      log_fatal ("[librvl write] DATA size is less than 0.");
      exit (EXIT_FAILURE);
    }

  rvl_write_file_sig (self);

  // Required chunks
  rvl_write_VHDR_chunk (self);
  rvl_write_GRID_chunk (self);
  rvl_write_DATA_chunk (self);

  if (self->text != NULL)
    {
      rvl_write_TEXT_chunk (self, self->text, self->numText);
    }

  rvl_write_VEND_chunk (self);
}
