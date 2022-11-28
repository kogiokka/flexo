#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "detail/rvl_log_p.h"
#include "detail/rvl_p.h"
#include "detail/rvl_text_p.h"

RVLText *
rvl_text_create ()
{
  RVLText *self = (RVLText *)calloc (1, sizeof (RVLText));
  return self;
}

void
rvl_text_destroy (RVLText **self)
{
  if (self == NULL)
    return;

  free (*self);
  *self = NULL;
}

void
rvl_text_set_field (RVLText *self, RVLenum tag, const char *value)
{
  self->tag = tag;

  u32 size    = strlen (value) + 1; // including '\0'
  self->value = (char *)malloc (size);
  memcpy (self->value, value, size);
  rvl_log_debug ("Creating text: 0x%.4X, %s.", self->tag, self->value);
}
