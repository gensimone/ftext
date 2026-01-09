#include "queue.h"
#include <stdio.h>

#define NEW_PAGE "\n %%%\n\n"

void format_page(char** page, Queue* stream_q, const int cols, const int lines, const int width,
                 const int gap);
void load_words(Queue* q, FILE* stream, const unsigned int width);
void print_page(char** page, FILE* stream, const unsigned int lines);
