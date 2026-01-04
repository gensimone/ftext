#include "alloc.h"
#include "strutil.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEW_PAGE "\n %%%\n\n"

char* next_word(void);
void ftext(int cols, const int lines, const int width, const int gap);

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

/* Basically the entry point.
   Based on the rules described in project.pdf, format the text from stdin
   and provide the formatted text in stdout.
 */
void ftext(const int cols, const int lines, const int width, const int gap)
{
  size_t page_width = (width * cols + (cols - 1) * gap) * 4;

  // save loaded words across pages to avoid missing
  // words when moving onto next page.
  int cap = 100; // capacity of the 'words' array
  int cw = 0;    // current width
  int wc = 0;    // word counter
  int eof = 0;   // notify EOF
  char** words = (char**) die_on_fail_calloc(cap, sizeof(char*));

  for (;;) {
    char** page = die_on_fail_palloc(lines, page_width);

    for (int c = 0; c < cols; c++) {    // Format individual page.
      for (int l = 0; l < lines; l++) { // Format an entire column.
        while (1) {                     // Format a line of a column.
          char* w = next_word();

          // EOF reached, flush any pending word and quit.
          if (w == NULL) {
            if (wc > 0) {
              char* line = die_on_fail_calloc(width, sizeof(char));
              // Insert gap before actual text and in columns that are
              // not the first. This way we prevent trailing whitespaces.
              if (c > 0)
                strcat(page[l], strspace(gap));

              // Concatenate the actual text.
              str_left_align(words, line, wc);
              strcat(page[l], line);

              free(line);
            }

            free(w);
            eof = 1;
            goto print_page;
          }

          // I think I need to calculate this because
          // otherwise the text is not align the way it should when
          // non ASCII characters are found inside the word.
          int wl = strlen(w) - count_notasciic(w);

          // New line means go to the next paragraph.
          if (wl == 0) {
            next_word(); // This will skip the empty line.
            char* line = die_on_fail_calloc(width, sizeof(char));
            str_left_align(words, line, wc);
            // Insert gap before actual text and in columns that are
            // not the first. This way we prevent trailing whitespaces.
            if (c > 0)
              strcat(page[l], strspace(gap));

            strcat(page[l], line);

            // Do not add whitespaces on the left side if we are
            // at the left most column.
            if (c < cols - 1)
              strcat(page[l], strspace(width - cw - wc + 1));

            // If we are at the bottom of the page or at
            // the left most column, do not insert a blank line.
            if (l < lines - 1 && c < cols - 1)
              strcat(page[l + 1], strspace(width));

            cw = 0;
            wc = 0;

            free(line);

            // Paragraphs are separated by a blank line so
            // go two lines forward.
            l++;
            break;
          }

          // Buffer exceeded, flush loaded words.
          else if (wl + cw + wc > width) {
            char* line = die_on_fail_calloc(width, sizeof(char));

            // Insert gap before actual text and in columns that are
            // not the first. This way we prevent trailing whitespaces.
            if (c > 0)
              strcat(page[l], strspace(gap));

            // Concatenate the actual text.
            str_border_align(words, line, wc, width, cw);
            strcat(page[l], line);

            // We need to fill width - len(word) with spaces when
            // only one word is going to be placed.
            if (wc == 1)
              strcat(page[l], strspace(width - strlen(words[0])));

            // Remember about the last word.
            cw = wl;
            wc = 1;
            words[0] = w;

            free(line);
            break; // go to next line.
          }

          else {
            // Capacity exceeded.
            if (wc >= cap)
              words = (char**) die_on_fail_realloc(words,
                                                   (cap *= 2) * sizeof(char*));

            // Save the word and move on until
            // we reach EOF or exceed the buffer.
            words[wc] = w;
            wc++;
            cw += wl;
          }
        }
      }
    }

  print_page:
    // Flush the entire page to stdout.
    for (int y = 0; y < lines; y++)
      printf("%s\n", page[y]);
    free_palloc(page, lines);

    if (eof)
      break;

    printf("%s", NEW_PAGE);
  }

  free_palloc(words, cap);
}
