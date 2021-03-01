#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void check_errors(int num_args, char** args);
void check_read_error();
void check_write_error();

int main(int argc, char** argv)
{
  check_errors(argc, argv);

  char* from = argv[1];
  char* to = argv[2];
  int len = strlen(from);

  // Get the first char from stdin
  char ch;
  ch = getchar();
  check_read_error();
  
  while (!feof(stdin))
  {
    // If the char is in the from set, transliterate it to the to set
    for (int i = 0; i < len; i++)
    {
      if (ch == from[i])
      {
	ch = to[i];
	break;
      }
    }

    // Write 'ch' to stdout, either transliterated or not
    putchar(ch);
    check_write_error();

    // Get the next char from stdin
    ch = getchar();
    check_read_error();
  }  

  return 0;
}

void check_errors(int num_args, char** args)
{
  // Check that there are 3 arguments
  if (num_args < 3)
  {
    fprintf(stderr, "Missing arguments. There should be 2 arguments.\n");
    exit(1);
  }

  if (num_args > 3)
  {
    fprintf(stderr, "Too many arguments. There should be 2 arguments.\n");
    exit(1);
  }

  char* from = args[1];
  char* to = args[2];

  // Check that the lengths of the two sets are the same
  int length_from = strlen(from);
  int length_to = strlen(to);
  if (length_from != length_to)
  {
    fprintf(stderr, "The two arguments should have the same length.\n");
    exit(1);
  }

  // Check that there are no duplicate chars in the FROM set
  for (int i = 0; i < length_from - 1; i++)
  {
    for (int k = i + 1; k < length_from; k++)
    {
      // If any character is different 
      if (from[i] == from[k])
      {
	fprintf(stderr, "The first argument (from set) must not contain any duplicate bytes.\n");
	exit(1);
      }
    }
  }
}

void check_read_error()
{
  if (ferror(stdin))
  {
    fprintf(stderr, "Error reading stdin\n");
    exit(1);
  }
}

void check_write_error()
{
  if (ferror(stdout))
  {
    fprintf(stderr, "Error writing to stdout\n");
    exit(1);
  }
}
