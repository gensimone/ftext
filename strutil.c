#include "alloc.h"
#include "queue.h"
#include <math.h>
#include <string.h>

int count_notasciic(char* str)
{
  int counter = 0;
  int in_non_ascii_char = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] < 0) {
      if (in_non_ascii_char == 0)
        in_non_ascii_char = 1;
      else
        counter++;
    } else
      in_non_ascii_char = 0;
  }
  return counter;
}

char* strspace(size_t n)
{
  char* str = die_on_fail_calloc(n, sizeof(char));
  memset(str, ' ', n);
  return str;
}

void sx_align(Queue* q, char* dst)
{
  char* word;
  while ((word = queue_pop(q)) != NULL) {
    strcat(dst, word);
    strcat(dst, " ");
  }
}

void bx_align(Queue* q, char* dst, int spaces)
{
  char* word;
  int tmp_spaces;
  while ((word = queue_pop(q)) != NULL) {
    strcat(dst, word);
    if (queue_size(q) == 0)
      break;
    tmp_spaces = ceil((double) spaces / queue_size(q));
    strcat(dst, strspace(tmp_spaces));
    spaces -= tmp_spaces;
  }
}
