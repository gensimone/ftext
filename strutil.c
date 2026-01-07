#include "alloc.h"
#include <math.h>
#include <string.h>

/*
  This is pure junk.
  Count all the excess bytes of non-ASCII chars.
  For example, 'è' results in 1 because it needs 2 bytes
  in UTF-8 to be represented.
  Why do I think I need this crap ? Well if we do not take
  into account this non-ASCII characters, lines that contains
  this characters will be displayed incorrectly.
  Here is an example:

    riga      di     ogni      commentare  le  linee      suddivisioni      del
    paragrafo   è   solo      di     codice    più      carico  di  lavoro in

  Notice how the text is not aligned. Now, using this function
  inside the core.c when calculating the word length the result is correct:

    riga      di     ogni      commentare  le  linee      suddivisioni      del
    paragrafo    è   solo      di     codice     più      carico  di  lavoro in
                 ^                                 ^
 */
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

/* Left align the specified words inside str. */
void str_left_align(char** ws, char* str, int wc)
{
  strcat(str, ws[0]);
  for (int i = 1; i < wc; i++) {
    strcat(str, " ");
    strcat(str, ws[i]);
  }
}

/* Allocate a string of size n full of
   spaces and return a pointer to it.
 */
char* strspace(size_t n)
{
  char* str = die_on_fail_calloc(n, sizeof(char));
  memset(str, ' ', n);
  return str;
}

/* Border alignment of words.
 */
void str_border_align(char** ws, char* str, int wc, int width,
                      int current_width)
{
  strcpy(str, ws[0]);
  int internal_words;
  if (wc == 1)
    internal_words = 0;
  else
    internal_words = wc - 2;
  int total_spaces = width - current_width;
  for (int i = 1; i < wc; i++) {
    int nspaces = ceil((double) total_spaces / (internal_words + 1));
    total_spaces -= nspaces;
    internal_words -= 1;
    char* spaces = strspace(nspaces);
    strcat(str, spaces);
    strcat(str, ws[i]);
  }
}
