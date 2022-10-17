#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <log.h>

#include "rvl.h"

#include "detail/rvl_p.h"

// .RVL FORMAT\0
RVLByte RVL_FILE_SIG[RVL_FILE_SIG_SIZE] = {
  131, 82, 86, 76, 32, 70, 79, 82, 77, 65, 84, 0,
};

static RVL *rvl_create (const char *filename, RVLIoState ioState);

RVL *
rvl_create_writer (const char *filename)
{
  RVL *rvl = rvl_create (filename, RVLIoState_Write);
  memset (&rvl->data, 0, sizeof (RVLData));
  rvl->writeFn = rvl_fwrite_default;
  return rvl;
}

RVL *
rvl_create_reader (const char *filename)
{
  RVL *rvl = rvl_create (filename, RVLIoState_Read);
  memset (&rvl->data, 0, sizeof (RVLData));
  rvl->readFn = rvl_fread_default;
  return rvl;
}

void
rvl_destroy (RVL **self)
{
  RVL *ptr = *self;
  rvl_text_destroy_array (&ptr->text);

  // Dealloc read buffer. Writer buffer pointer is non-owning so the user is
  // responsible for calling this dealloc function.
  rvl_dealloc (ptr, &ptr->data.rbuf);
  rvl_dealloc (ptr, &ptr->grid.vxDimBuf);

  fclose (ptr->io);
  free (ptr);

  *self = NULL;
}

void
rvl_alloc (RVL *self, RVLByte **ptr, RVLSize size)
{
  assert (self != NULL || ptr != NULL);

  if (*ptr != NULL)
    {
      free (*ptr);
    }

  *ptr = (RVLByte *)calloc (1, size);

  if (*ptr == NULL)
    {
      log_fatal ("[librvl alloc] Memory allocation failure.\n");
      exit (EXIT_FAILURE);
    }
}

void
rvl_dealloc (RVL *self, RVLByte **ptr)
{
  assert (self != NULL || ptr != NULL);

  if (*ptr == NULL)
    return;

  free (*ptr);
  *ptr = NULL;
}

RVL *
rvl_create (const char *filename, RVLIoState ioState)
{

  log_set_level (LOG_INFO);

#ifndef NDEBUG
  log_set_level (LOG_TRACE);
#endif

  RVL *self        = (RVL *)calloc (1, sizeof (RVL));
  self->version[0] = RVL_VERSION_MAJOR;
  self->version[1] = RVL_VERSION_MINOR;
  self->ioState    = ioState;
  self->text       = NULL;
  self->numText    = 0;

  switch (ioState)
    {
    case RVLIoState_Read:
      self->io = fopen (filename, "rb");
      break;
    case RVLIoState_Write:
      self->io = fopen (filename, "wb");
      break;
    }

  if (self->io == NULL)
    {
      log_error ("[rvl io] %s", strerror (errno));
    }

  return self;
}
