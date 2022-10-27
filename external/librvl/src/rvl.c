#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <log.h>

#include "rvl.h"

#include "detail/rvl_p.h"
#include "detail/rvl_text_p.h"

// .RVL FORMAT\0
BYTE RVL_FILE_SIG[RVL_FILE_SIG_SIZE] = {
  131, 82, 86, 76, 32, 70, 79, 82, 77, 65, 84, 0,
};

static RVL *rvl_create (RVLIoState ioState);

RVL *
rvl_create_writer (void)
{
  RVL *rvl = rvl_create (RVLIoState_Write);
  rvl->writeFn = rvl_fwrite_default;
  return rvl;
}

RVL *
rvl_create_reader (void)
{
  RVL *rvl = rvl_create (RVLIoState_Read);
  rvl->readFn = rvl_fread_default;
  return rvl;
}

void
rvl_destroy (RVL **self)
{
  if (*self == NULL)
    return;

  RVL *ptr = *self;

  // Dealloc read buffer. Writer buffer pointer is non-owning so the user is
  // responsible for calling this dealloc function.
  rvl_dealloc (ptr, &ptr->data.rbuf);
  rvl_dealloc (ptr, &ptr->grid.dimBuf);

  if (ptr->text != NULL)
    {
      RVLText *cur = ptr->text;
      while (cur->next != NULL)
        {
          RVLText *tmp = cur;
          cur          = cur->next;
          free (tmp->value);
          free (tmp);
        }
    }

  if (ptr->io != NULL)
    {
      fclose (ptr->io);
    }

  free (ptr);

  *self = NULL;
}

void
rvl_alloc (RVL *self, BYTE **ptr, u32 size)
{
  assert (self != NULL || ptr != NULL);

  if (*ptr != NULL)
    {
      free (*ptr);
    }

  *ptr = (BYTE *)calloc (1, size);

  if (*ptr == NULL)
    {
      log_fatal ("[librvl alloc] Memory allocation failure.\n");
      exit (EXIT_FAILURE);
    }
}

void
rvl_dealloc (RVL *self, BYTE **ptr)
{
  assert (self != NULL || ptr != NULL);

  if (*ptr == NULL)
    return;

  free (*ptr);
  *ptr = NULL;
}

RVL *
rvl_create (RVLIoState ioState)
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

  // Explicitly set the default values of the optional settings.
  self->compress         = RVL_COMPRESSION_LZMA;
  self->grid.unit        = RVL_UNIT_NA;
  self->grid.position[0] = 0.0f;
  self->grid.position[1] = 0.0f;
  self->grid.position[2] = 0.0f;

  return self;
}

