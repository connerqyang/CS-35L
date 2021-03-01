#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void check_read_error(ssize_t flag);
void check_write_error(ssize_t flag);

int main(int argc, char** argv)
{
  // Get parameters
  char* from = argv[1];
  char* to = argv[2];

  // Check the number of arguments
  if (argc < 3)
  {
    char* error_msg = "Not enough arguments. There should be 2 arguments.\n";
    write(2, error_msg, sizeof(error_msg));
    exit(1);
  }

  if (argc > 3)
  {
    char* error_msg = "Too many arguments. There should be 2 arguments.\n";
    write(2, error_msg, sizeof(error_msg));
    exit(1);
  }

  int len_from = strlen(from);
  int len_to = strlen(to);

  // Check that the length of both sets are the same
  if (len_from != len_to)
  {
    char* error_msg = "The length of both sets should be the same.\n";
    write(2, error_msg, sizeof(error_msg));
    exit(1);
  }

  int length = len_to;
  // Check that there are no duplicate bytes in the from set
  for (int i = 0; i < length - 1; i++)
  {
    for (int k = i + 1; k < length; k++)
    {
      if (from[i] == from[k])
      {
	char* error_msg = "There should be no duplicate bytes in the from set\n";
	write(2, error_msg, sizeof(error_msg));
	exit(1);
      }
    }
  }

  // Read in first character from stdin, using system call
  char ch[1];
  ssize_t flag;
  flag = read(0, ch, 1);
  check_read_error(flag);
  
  // Iteratively read and write using system calls for each character
  while (flag != 0 && flag != -1)
  {
    // If needed, transliterate ch
    for (int i = 0; i < length; i++)
    {
      if (ch[0] == from[i])
      {
	ch[0] = to[i];
	break;
      }
    }

    // Write the character, transliterated or not
    flag = write(1, ch, 1);
    check_write_error(flag);

    // Read the next character with a system call
    flag = read(0, ch, 1);
    check_read_error(flag);
  }
  return 0;
}

void check_read_error(ssize_t flag)
{
  if (flag == -1)
  {
    char* error_msg = "Error reading from stdin.\n";
    write(2, error_msg, sizeof(error_msg));
    exit(1);
  }
}

void check_write_error(ssize_t flag)
{
  if (flag == -1)
  {
    char* error_msg = "Error writing to stdout.\n";
    write(2, error_msg, sizeof(error_msg));
    exit(1);
  }
}
