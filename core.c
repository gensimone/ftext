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

/* Load the word from stream into q. */
void load_words(Queue* q, FILE* stream)
{
  char* word;
  short in_paragraph_divider = 0;
  while ((word = next_word(stream)) != NULL) {
    if (strlen(word) > 0) {
      in_paragraph_divider = 0;
      queue_push(q, word);
    } else {
      if (in_paragraph_divider == 0)
        queue_push(q, word);
      in_paragraph_divider = 1;
    }
  }
}

/*
 Break a large word in multiple releated words.
*/
void insert_word(char* src, const unsigned int width, Queue* q)
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

/*
  Format a single page.
  Return 1 if reach EOF otherwise 0.
 */
static int format_page(char** page, Queue* stream_q, Queue* inter_q, const unsigned int cols,
                       const unsigned int lines, const unsigned int width, const unsigned int gap)
{
  unsigned short EOF_reached = 0;
  for (unsigned int c = 0; c < cols; c++) {
    for (unsigned int l = 0; l < lines; l++) {
      if (EOF_reached) {
        if (queue_size(inter_q) == 0) // We are done!
          goto quit;
      } else {
        char* word;
        // Read until the buffer is full.
        while ((word = queue_pop(stream_q)) != NULL && queue_length(inter_q) <= width)
          insert_word(word, width, inter_q);

        // We have reached the end, EOF notification but do not exit yet,
        // maybe we have words left to write.
        if (word == NULL)
          EOF_reached = 1;
        else
          insert_word(word, width, inter_q); // This was taken, but not inserted.
      }

      // Consider only the first words that fit within the width.
      Queue* tmp_q = queue_create();
      unsigned short new_paragraph = 0;
      do {
        char* word = queue_pop(inter_q);
        if (strlen(word) == 0) {
          new_paragraph = 1;
          free(word);
          break;
        }
        queue_push(tmp_q, word);
      } while (queue_size(inter_q) > 0 &&
               queue_length(tmp_q) + queue_size(tmp_q) + strlen(queue_head(inter_q)) <= width);

      // Gap
      if (c > 0) {
        char* spaces = strspace(gap);
        strcat(page[l], spaces);
        free(spaces);
      }

      // Calculate spaces here before consuming tmp_q.
      // These are extra spaces added to the right of the line when, for example, there is only one
      // word (the last line of a paragraph) or non-ASCII characters are present.
      char* extra_spaces = strspace(width - (queue_size(tmp_q) == 1 ? queue_length(tmp_q) : width));

      char* line = die_on_fail_calloc((width * 4) + 1, sizeof(char));
      if (new_paragraph) {
        sx_align(tmp_q, line);
        queue_top(inter_q, strspace(width));
      } else
        bx_align(tmp_q, line, width - queue_length(tmp_q));
      assert(queue_size(tmp_q) == 0);

      strcat(page[l], line);
      strcat(page[l], extra_spaces);

      free(extra_spaces);
      free(line);
      free(tmp_q);
    }
  }
quit:
  return EOF_reached;
}

/*
  Format the text from input_stream to output_stream
  based on the specified parameters.
*/
void format_pages(FILE* input_stream, FILE* output_stream, const int cols, const int lines,
                  const int width, const int gap)
{
  Queue* stream_q = queue_create();
  load_words(stream_q, input_stream);

  const size_t page_width = (width * cols + (cols - 1) * gap) * 4;

  Queue* inter_q = queue_create();
  char** page;
  int EOF_reached;
  do {
    page = die_on_fail_palloc(lines, page_width);
    EOF_reached = format_page(page, stream_q, inter_q, cols, lines, width, gap);
    for (int i = 0; i < lines; i++)
      fprintf(output_stream, "%s\n", page[i]);
    if (!EOF_reached)
      fprintf(output_stream, "%s", NEW_PAGE);
    free_page(page, lines);
  } while (!EOF_reached);

  assert(queue_size(inter_q) == 0);
  assert(queue_size(stream_q) == 0);

  free(inter_q);
  free(stream_q);
}
