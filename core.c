#include "alloc.h"
#include "strutil.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEW_PAGE "\n %%%\n\n"

char* next_word(void);
int fpage(char** page, int cols, int lines, int width, int gap);
void ftext(int cols, int lines, int width, int gap);

/*
  Read from stdin and return the first encountered word.
  Return null character if, before founding any non-space
  character, a \n is encountered. This is useful as a way
  to recognized new lines without polluting the words.
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
        if (len + 1 >= cap) {
            cap *= 2; // it may be overkill.
            char* tmp = realloc(buf, cap);
            if (!tmp) {
                free(buf);
                return NULL;
            }
            buf = tmp;
        }

        buf[len++] = (char) c;
        c = getchar();
    }
    buf[len] = '\0';
    return buf;
}

void ftext(int cols, int lines, int width, int gap)
{
    size_t page_width = (width * cols + (cols - 1) * gap) * 4;
    for (;;) {
        char** page = die_on_fail_palloc(lines, page_width);
        int status = fpage(page, cols, lines, width, gap);
        for (int y = 0; y < lines; y++)
            printf("%s\n", page[y]);
        free_palloc(page, lines);
        if (status == EOF)
            break;
        printf("%s", NEW_PAGE);
    }
}

int fpage(char** page, int cols, int lines, int width, int gap)
{
    int max_words = 100;
    char** words = (char**) die_on_fail_calloc(max_words, sizeof(char*));
    unsigned int current_width = 0;
    int wc = 0;
    for (int c = 0; c < cols; c++) {
        for (int l = 0; l < lines; l++) {
            while (1) {
                char* w = next_word();

                // EOF Reached.
                if (w == NULL) {
                    if (wc > 0) {
                        char* line = die_on_fail_calloc(width, sizeof(char));
                        // alignment
                        str_left_align(words, line, wc);
                        strcat(page[l], line);
                        free(line);
                    }
                    free(w);
                    free_palloc(words, max_words);
                    return EOF;
                }

                int wl = strlen(w) - count_notascii(w);
                // New line found.
                if (wl == 0) {
                    next_word();
                    char* line = die_on_fail_calloc(width, sizeof(char));
                    str_left_align(words, line, wc);
                    strcat(page[l], line);
                    strcat(page[l], strspace(gap + width - current_width - wc + 1));
                    if (l < lines - 1)
                        strcat(page[l + 1], strspace(width + gap));
                    current_width = 0;
                    wc = 0;
                    words[0] = NULL;
                    l++;
                    free(line);
                    break;
                }
                // Buffer has no space for 'word'.
                else if (wl + current_width + wc > width) {
                    char* line = die_on_fail_calloc(width, sizeof(char));
                    str_border_align(words, line, wc, width, current_width);
                    strcat(page[l], line);
                    strcat(page[l], strspace(gap));
                    current_width = wl;
                    wc = 1;
                    words[0] = w;
                    free(line);
                    break;
                }
                // Buffer has enough space for 'word'.
                else {
                    if (wc >= max_words) {
                        max_words += 100;
                        words = (char**) die_on_fail_realloc(words, max_words * sizeof(char*));
                    }
                    words[wc] = w;
                    wc++;
                    current_width += wl;
                }
            }
        }
    }
    free_palloc(words, lines);
    return 0;
}
