#include <stdio.h>
#include <string.h>

void rvl_test_write_INFO ();
void rvl_test_read_INFO ();
void rvl_test_write_DATA ();
void rvl_test_read_DATA ();
void rvl_test_write_TEXT ();
void rvl_test_read_TEXT ();

void rvl_test_write();
void rvl_test_read_parts ();

int
main (int argc, char *argv[])
{
  if (strcmp (argv[1], "1") == 0)
    {
      rvl_test_write_INFO ();
    }
  else if (strcmp (argv[1], "2") == 0)
    {
      rvl_test_write_DATA ();
    }
  else if (strcmp (argv[1], "3") == 0)
    {
      rvl_test_write_TEXT ();
    }
  else if (strcmp (argv[1], "4") == 0)
    {
      rvl_test_read_INFO ();
    }
  else if (strcmp (argv[1], "5") == 0)
    {
      rvl_test_read_DATA ();
    }
  else if (strcmp (argv[1], "6") == 0)
    {
      rvl_test_read_TEXT ();
    }
  else if (strcmp (argv[1], "7") == 0)
    {
      rvl_test_write();
    }
  else if (strcmp (argv[1], "8") == 0)
    {
      rvl_test_read_parts();
    }

  return 0;
}
