#include "queue.h"
#include <pthread.h>
#include <stdio.h>

struct load_words_args {
  Queue* q;
  FILE* stream;
  unsigned int width;
};

struct format_page_args {
  pthread_t tid;
  char** page;
  Queue* stream_q;
  unsigned int cols;
  unsigned int lines;
  unsigned int width;
  unsigned int gap;
};

struct print_page_args {
  pthread_t tid;
  FILE* stream;
  char** page;
  unsigned int lines;
};

void* load_words_thread(void* args);
void* format_page_thread(void* args);
void* print_page_thread(void* args);
void mthread_exec(FILE* input_stream, FILE* output_stream, const unsigned cols,
                  const unsigned lines, const unsigned width, const unsigned gap);
void sthread_exec(FILE* input_stream, FILE* output_stream, const unsigned cols,
                  const unsigned lines, const unsigned width, const unsigned gap);
