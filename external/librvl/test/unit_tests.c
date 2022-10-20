#include <stdio.h>
#include <string.h>

void rvl_test_write_regular_grid ();
void rvl_test_read_regular_grid ();
void rvl_test_write_rectilinear_grid ();
void rvl_test_read_rectilinear_grid ();
void rvl_test_partially_read ();
void rvl_test_uninitialized_rvl ();

int
main (int argc, char *argv[])
{
  if (strcmp (argv[1], "1") == 0)
    {
      rvl_test_write_regular_grid ();
    }
  else if (strcmp (argv[1], "2") == 0)
    {
      rvl_test_read_regular_grid ();
    }
  else if (strcmp (argv[1], "3") == 0)
    {
      rvl_test_write_rectilinear_grid ();
    }
  else if (strcmp (argv[1], "4") == 0)
    {
      rvl_test_read_rectilinear_grid ();
    }
  else if (strcmp (argv[1], "5") == 0)
    {
      rvl_test_partially_read ();
    }
  else if (strcmp (argv[1], "6") == 0)
    {
      rvl_test_uninitialized_rvl ();
    }

  return 0;
}
