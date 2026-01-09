#include "alloc.h"
#include "queue.h"
#include "strutil.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEW_PAGE "\n %%%\n\n"

/*
  Read from the stream and return the first word.
  If '\n' is found, return a pointer to '\0'.
  If EOF is found, return NULL.
 */
char* next_word(FILE* stream)
{
  int c;
  size_t len = 0;
  size_t cap = 100;
  char* buf = die_on_fail_calloc(cap, sizeof(char));

  while ((c = fgetc(stream)) != EOF && isspace(c)) {
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
    if (len + 1 >= cap) // capacity exceeded.
      buf = die_on_fail_realloc(buf, cap *= 2);

    buf[len++] = (char) c;
    c = fgetc(stream);
  }

  buf[len] = '\0';
  return buf;
}

/*
 Break a large word in multiple releated words.
*/
static void insert_word(Queue* q, char* src, const unsigned int width)
{
  unsigned int src_length = strlen(src);
  if (src_length <= width)
    queue_push(q, src);
  else {
    char* chunk;
    size_t inc_width = 0;
    do {
      chunk = (char*) die_on_fail_calloc(width + 1, sizeof(char));
      strncpy(chunk, src + inc_width, width);
      queue_push(q, chunk);
      inc_width += width;
    } while (src_length > inc_width);
    free(src);
  }
}

/* Load the word from stream into q. */
void load_words(Queue* q, FILE* stream, const unsigned int width)
{
  char* word;
  short in_paragraph_divider = 0;
  while ((word = next_word(stream)) != NULL) {
    if (strlen(word) > 0) {
      in_paragraph_divider = 0;
      insert_word(q, word, width);
    } else {
      if (in_paragraph_divider == 0)
        queue_push(q, word);
      in_paragraph_divider = 1;
    }
  }
}

static void format_page(char** page, Queue* stream_q, const unsigned int cols,
                        const unsigned int lines, const unsigned int width, const unsigned int gap)
{
  for (unsigned int c = 0; c < cols; c++) {
    for (unsigned int l = 0; l < lines; l++) {
      if (queue_size(stream_q) == 0)
        return;

      if (is_all_whitespace(queue_head(stream_q))) { // Empty line here.
        char* w = queue_pop(stream_q);
        free(w);
        continue;
      }

      // Consider only the first words that fit within the width.
      Queue* line_q = queue_create();
      unsigned short new_paragraph = 0;
      while (queue_size(stream_q) > 0 &&
             queue_length(line_q) + rstrlen(queue_head(stream_q)) +
                     (strlen(queue_head(stream_q)) > 0 ? queue_size(line_q)
                                                       : queue_size(line_q) - 1) <=
                 width) {
        char* word = queue_pop(stream_q);
        if (strlen(word) == 0) {
          new_paragraph = 1;
          free(word);
          break;
        }
        queue_push(line_q, word);
      }

      int nspaces = (width * c + c * gap) - rstrlen(page[l]);
      if (nspaces > 0) {
        char* spaces = strspace(nspaces);
        strcat(page[l], spaces);
        free(spaces);
      }

      char* line = die_on_fail_calloc((width * 4) + 1, sizeof(char));
      if (new_paragraph) {
        sx_align(line_q, line);
        queue_top(stream_q, strspace(width)); // Placeholder for new paragraph line.
      } else
        bx_align(line_q, line, width - queue_length(line_q));

      assert(queue_size(line_q) == 0);

      strcat(page[l], line);

      free(line);
      free(line_q);
    }
  }
}

/*
  Format the text from input_stream to output_stream
  based on the specified parameters.
*/
void format_pages(FILE* input_stream, FILE* output_stream, const int cols, const int lines,
                  const int width, const int gap)
{
  Queue* stream_q = queue_create();
  load_words(stream_q, input_stream, width);

  const size_t page_width = (width * cols + (cols - 1) * gap) * 4;
  char** page;
  do {
    page = die_on_fail_palloc(lines, page_width);
    format_page(page, stream_q, cols, lines, width, gap);
    for (int i = 0; i < lines; i++)
      fprintf(output_stream, "%s\n", page[i]);
    if (queue_size(stream_q) > 0)
      fprintf(output_stream, "%s", NEW_PAGE);
    free_page(page, lines);
  } while (queue_size(stream_q) > 0);
  assert(queue_size(stream_q) == 0);
  free(stream_q);
}
