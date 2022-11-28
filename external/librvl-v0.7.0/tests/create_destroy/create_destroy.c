#include <rvl.h>

int
main ()
{
  RVL *rvl = NULL;
  rvl_destroy (&rvl);

  rvl = rvl_create_reader ();
  rvl_destroy (&rvl);

  rvl = rvl_create_writer ();
  rvl_destroy (&rvl);

  return 0;
}
