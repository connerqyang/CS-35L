#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

void freeMemory(char* buf, char** words, int num_words);
int frobcmp(const char* a, const char* b);
int cmp_wrapper(const void* a, const void* b);
int frobcmp_f(const char* a, const char* b);
int cmp_f_wrapper(const void* a, const void* b);
const char deobfuscate(const char ch);

int main(int argc, char** argv)
{
  // Check arguments/options
  int used_option_f;
  if (argc == 1)
  {
    used_option_f = 0;
  }
  else if (argc == 2)
  {
    // Check for correct option
    char* option = argv[1];
    if (strcmp("-f", option) != 0)
    {
      fprintf(stderr, "Too many arguments passed in.\n");
      exit(1);
    }
    else
    {
      used_option_f = 1;
    }
  }
  else
  {
    fprintf(stderr, "Too many arguments passed in.\n");
    exit(1);
  }

  struct stat buf_stat;
  if (fstat(0, &buf_stat) == -1)
  {
    fprintf(stderr, "Error with fstat.\n");
    exit(1);
  }

  const int DEFAULT_INITIAL_SIZE = 8192;
  // If input is a regular file, allocate memory for the file
  // Otherwise, allocate 8 KiB, or 8192 bytes
  long buffer_size = S_ISREG(buf_stat.st_mode) ? buf_stat.st_size : DEFAULT_INITIAL_SIZE;

  // Copy input into char array 'file'
  char* buf = (char*)malloc(sizeof(char) * buffer_size);
  // Check memory allocation error
  if (buf == NULL)
  {
    fprintf(stderr, "Memory allocation error.\n");
    exit(1);
  }

  // Read in using system calls into the buffer
  int num_bytes = 0;
  char* temp;
  while (1)
  {
    // If the buffer is more than half full, resize buffer to 2x
    if (num_bytes > buffer_size / 2)
    {
      // Update buffer size
      buffer_size = buffer_size * 2;
      buf = (char*)realloc(buf, sizeof(char) * buffer_size);
      if (buf == NULL)
      {
        fprintf(stderr, "Memory allocation error.\n");
        exit(1);
      }
    }
    
    int num_bytes_read = read(0, buf + num_bytes, buffer_size - num_bytes);
    // If flag is -1, error reading
    if (num_bytes_read == -1)
    {
      free(buf);
      fprintf(stderr, "Memory allocation error.\n");
      exit(1);
    }
    // Stop reading input for EOF, zero-byte input streams
    if (num_bytes_read == 0)
    {
      break;
    }
    
    num_bytes += num_bytes_read;
  }

  // Organize the file into words
  int num_words = 0;
  int word_size = 0;
  
  char** words = (char**)malloc(sizeof(char*));
  if (words == NULL)
  {
    freeMemory(buf, words, num_words);
    fprintf(stderr, "Memory allocation error.\n");
    exit(1);
  }
  char* word = malloc(sizeof(char));
  if (word == NULL)
  {
    freeMemory(buf, words, num_words);
    fprintf(stderr, "Memory allocation error.\n");
    exit(1);
  }

  char ch;

  // Iteratively parse through buffer
  for (int i = 0; i < num_bytes; i++)
  {
    // Read in char by char from buffer
    ch = buf[i];

    // If ch is a regular character (not ' ')
    if (ch != ' ')
    {
      word[word_size] = ch;
      word_size++;   // Increment the word size, as we have added one character
      word = realloc(word, sizeof(char) * (word_size + 1));

      // Check for memory allocation failure
      if (word == NULL)
      {
	freeMemory(buf, words, num_words);
	fprintf(stderr, "Memory allocation error.\n");
	exit(1);
      }
    }

    // Check if this char is the end of the word, or the very last char
    if (word_size > 0 && (ch == ' ' || i == num_bytes - 1))
    {
      // I append the ' ' regardless, because this handles the case of no trailing space on the last word
      word[word_size] = ' ';   // Append a ' ' character
      words[num_words] = word; // Add current word to the words array
      num_words++;             // Increment the number of words accordingly

      // Allocate memory for the next word
      words = realloc(words, sizeof(char*) * (num_words + 1));
      word = malloc(sizeof(char));

      // Check for memory allocation error
      if (words == NULL || word == NULL)
      {
	freeMemory(buf, words, num_words);
	fprintf(stderr, "Memory allocation error.\n");
	exit(1);
      }
      
      word_size = 0;   // Reset size of the current word (in characters) to 0
    }
  }

  free(word);   // Since realloc is only called when a new word is discovered,   
                // the final word was never freed, so free it now

  // Sort, use different comparison functions for option -f
  if (!used_option_f)
    qsort(words, num_words, sizeof(char*), cmp_wrapper);
  else
    qsort(words, num_words, sizeof(char*), cmp_f_wrapper);

    // Print to stdout
  for (int i = 0; i < num_words; i++)
  {
    for (int j = 0; j < num_words; j++)
    {
      int write_flag = write(1, &words[i][j], 1);
      if (write_flag == -1)
      {
        freeMemory(buf, words, num_words);
	fprintf(stderr, "Error writing to stdout.\n");
        exit(1);
      }

      // If the character is a space, move on to next word
      if (words[i][j] == ' ')
	break;
    }
  }
  
  freeMemory(buf, words, num_words);
  return 0;
}

void freeMemory(char* buf, char** words, int num_words)
{
  if (buf != NULL)
    free(buf);
  
  // Avoid double-deleting
  if (words == NULL)
    return;

  // Free all dynamically-allocated memory
  int i = 0;
  while(i < num_words)
  {
    // Making sure I am not double deleting
    if (words[i] == NULL)
      continue;
    free(words[i]);
    i++;
  }
  free(words);
}

// Comparison function implementation
int frobcmp(const char* a, const char* b)
{
  // Iterate through each byte of a and b. If a and b are both equal all the way until ' ', return 0
  while (*a == *b)
  {
    if (*b == ' ')
      return 0;
    a++;
    b++;   // Now, a and b will be at an unequal char if we did not return.      
  }

  // Otherwise, return 1 if the deobfuscated a is greater than the deobfuscated b, or if
  // b is ' ', and return -1 if neither of those 2 conditions are true.          
  if ( (deobfuscate(*a) > deobfuscate(*b)) || (*b == ' ') )
    return 1;
  else
    return -1;
}

// Wrapper to pass in comparison function to qsort
int cmp_wrapper(const void* a, const void* b)
{
  return frobcmp(*(const char**)a, *(const char**)b);
}

// Comparison function implementation for -f option
int frobcmp_f(const char* a, const char* b)
{
  // Iterate through each byte of a and b. If a and b are both equal all the way until '' , return 0
  while (*a == *b)
  {
    if (*b == ' ')
      return 0;
    a++;
    b++;   // Now, a and b will be at an unequal char if we did not return.
  }

  unsigned char de_a = (unsigned char) deobfuscate(*a);
  unsigned char de_b = (unsigned char) deobfuscate(*b);
  // Otherwise, return 1 if the deobfuscated a is greater than the deobfuscated b, or if
  // b is ' ', and return -1 if neither of those 2 conditions are true.
  if ( (toupper(de_a) > toupper(de_b)) || (*b == ' ') )
    return 1;
  else
    return -1;
}

// Wrapper to pass in comparison function to qsort for -f option
int cmp_f_wrapper(const void* a, const void* b)
{
  return frobcmp_f(*(const char**)a, *(const char**)b);
}

// Returns the deobfuscated argument
const char deobfuscate(const char ch)
{
  return (ch ^ 42);
}
