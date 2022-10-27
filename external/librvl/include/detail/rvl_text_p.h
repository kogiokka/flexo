#ifndef RVL_TEXT_P_H
#define RVL_TEXT_P_H

#ifndef RVL_H_INTERNAL
#error Never include this file directly. Use <rvl.h> instead.
#endif

#include <stdint.h>

#include "detail/rvl_p.h"

typedef uint8_t RVLTag;

struct RVLText
{
  RVLenum tag;
  char   *value;

  struct RVLText *next;
};

RVLText *rvl_text_create ();
void     rvl_text_destroy (RVLText **self);
void     rvl_text_set_field (RVLText *self, RVLenum tag, const char *value);

#endif
