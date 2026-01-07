#include "alloc.h"
#include "queue.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Read from stdin and return the first encountered word.
  Return null character if, before founding any non-space
  character, a \n is encountered. This is useful as a way
  to recognized new lines without polluting the words with
  escape sequences.
 */
char* next_word(void)
{
  int c;
  size_t len = 0;
  size_t cap = 100; // more than enough.
  char* buf = die_on_fail_calloc(cap, sizeof(char));

  // skip all escape sequences except \n.
  while ((c = getchar()) != EOF && isspace(c)) {
    if (c == '\n') {
      buf[0] = '\0';
      return buf;
    }
  }

  if (c == EOF) {
    free(buf);
    return NULL;
  }

  while (c != EOF && !isspace(c)) {
    // capacity exceeded, need realloc.
    if (len + 1 >= cap)
      buf = die_on_fail_realloc(buf, cap *= 2);

    buf[len++] = (char) c;
    c = getchar();
  }
  buf[len] = '\0';
  return buf;
}

void ftext(int cols, const int lines, const int width, const int gap)
{
  Queue* words_queue = queue_create();
  int total_length = 0; // total length of the words inside the words_queue.
  size_t page_width = (width * cols + (cols - 1) * gap) * 4;
  while (1) {
    char** page = die_on_fail_palloc(lines, page_width);
    for (int c = 0; c < cols; c++) {    // format individual page.
      for (int l = 0; l < lines; l++) { // format an entire column.
        while (1) {                     // format a line of a column.
          if (total_length + queue_size(words_queue) - 1 >= width) {
            Queue* tmp_queue = queue_create();
            int tmp_length = 0;
            while (tmp_length < width) {
              int word_length = strlen(queue_head(words_queue));
              if (word_length + tmp_length + queue_size(tmp_queue) <= width)
                queue_push(tmp_queue, queue_pop(words_queue));
              tmp_length += word_length;
            }
          } else {
            char* word = next_word();
            queue_push(words_queue, word);
          }
        }
      }
    }
  }
}
