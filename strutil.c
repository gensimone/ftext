#include "alloc.h"
#include "queue.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

unsigned int rstrlen(const char* str)
{
  if (strlen(str) == 0)
    return 0;

  int counter = 0;
  int in_nonascii_char = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] < 0) {
      if (in_nonascii_char == 0)
        in_nonascii_char = 1;
      else
        counter++;
    } else
      in_nonascii_char = 0;
  }

  return strlen(str) - counter;
}

char* strspace(size_t n)
{
  char* str = die_on_fail_calloc(n + 1, sizeof(char));
  memset(str, ' ', n);
  str[n] = '\0';
  return str;
}

void sx_align(Queue* q, char* dst)
{
  char* word;
  while ((word = queue_pop(q)) != NULL) {
    strcat(dst, word);
    free(word);
    if (queue_size(q) > 0)
      strcat(dst, " ");
  }
}

void bx_align(Queue* q, char* dst, int nspaces)
{
  char* word;
  int tmp_spaces;
  while ((word = queue_pop(q)) != NULL) {
    strcat(dst, word);
    free(word);
    if (queue_size(q) == 0)
      break;
    tmp_spaces = ceil((double) nspaces / queue_size(q));
    char* spaces = strspace(tmp_spaces);
    strcat(dst, spaces);
    free(spaces);
    nspaces -= tmp_spaces;
  }
}
