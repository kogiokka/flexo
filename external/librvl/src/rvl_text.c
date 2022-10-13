#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "detail/rvl_p.h"
#include "detail/rvl_text_p.h"

RVLText *
rvl_text_create_array (int num)
{
  RVLText *textarr = (RVLText *)calloc (1, sizeof (RVLText) * num);
  return textarr;
}

void
rvl_text_destroy_array (RVLText **self)
{
  if (*self == NULL)
    {
      return;
    }

  free ((*self)->value);
  free (*self);
  *self = NULL;
}

// key and value must be null-terminated.
void
rvl_text_set (RVLText *textArr, int index, char *key, char *value)
{
  RVLText *text = &textArr[index];

  size_t keySize   = strlen (key);
  size_t valueSize = strlen (value);

  if (keySize >= 80)
    {
      fprintf (stderr,
               "[WARNING] Key length exceeds the maximum limit and will "
               "be truncated:\n \"%s\"\n",
               key);
      keySize = 79;
    }
  if (valueSize > sizeof (RVLSize) - (keySize + 1))
    {
      fprintf (stderr,
               "[WARNING] Value length exceeds the maximum limit and will "
               "be truncated:\n \"%s\"\n",
               value);
      valueSize = sizeof (RVLSize) - (keySize + 1);
    }

  memcpy (text->key, key, keySize);
  text->key[keySize] = '\0';

  text->value = (char *)malloc (valueSize + 1);
  strcpy (text->value, value);
}

void
rvl_text_get (RVLText *textArr, int index, const char **key,
              const char **value)
{
  *key   = textArr[index].key;
  *value = textArr[index].value;
}
