#include "thread.h"
#include "alloc.h"
#include "core.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
  P-Thread wrapper function.
  This one doesn't wait for anything before being executed.
 */
void* load_words_thread(void* targs)
{
  struct load_words_args* args = (struct load_words_args*) targs;
  load_words(args->q, args->stream, args->width);
  return NULL;
}

/*
  P-Thread wrapper function.
  Wait targs->tid (generally load_words_thread) and then invoke 'format_page'.
 */
void* format_page_thread(void* targs)
{
  struct format_page_args* args = (struct format_page_args*) targs;
  pthread_join(args->tid, NULL); // join the load words thread
  format_page(args->page, args->stream_q, args->cols, args->lines, args->width, args->gap);
  return NULL;
}

/*
  P-Thread wrapper function.
  Wait targs->tid (generally format_page_thread) and then invoke 'print_page'.
 */
void* print_page_thread(void* targs)
{
  struct print_page_args* args = (struct print_page_args*) targs;
  pthread_join(args->tid, NULL); // join the format page thread
  print_page(args->page, args->stream, args->lines);
  return NULL;
}

/* Multiple thread execution.*/
void mthread_exec(FILE* input_stream, FILE* output_stream, const unsigned cols,
                  const unsigned lines, const unsigned width, const unsigned gap)
{
  /* Create the 'load words' thread, which reads from the specified stream */
  /* (stdin in this case) and loads any word into a queue. That queue will */
  /* be used later to format the pages. */

  pthread_t lw_tid;
  Queue* stream_q = queue_create();
  struct load_words_args lw_args = {stream_q, input_stream, width};
  pthread_create(&lw_tid, NULL, load_words_thread, (void*) &lw_args);

  /* -- Formatting and printing. -- */

  const size_t page_width = (width * cols + (cols - 1) * gap) * 4;

  do {

    /* Allocate a page in memory. */

    char** page = die_on_fail_palloc(lines, page_width);

    /* Create the 'format page' thread, which formats a single page. */
    /* In order to format the page, this thread needs to await the 'load words' thread. */

    pthread_t fp_tid; // format page TID
    struct format_page_args fp_args = {lw_tid, page, stream_q, cols, lines, width, gap};
    pthread_create(&fp_tid, NULL, format_page_thread, (void*) &fp_args);

    /* Createa the 'print page' thread, which writes the page to the specified stream */
    /* (stdout in this case). In order to write the page, this thread needs to await the 'format
     * page' thread. */

    pthread_t pp_tid; // print page TID
    struct print_page_args pp_args = {fp_tid, output_stream, page, lines};
    pthread_create(&pp_tid, NULL, print_page_thread, (void*) &pp_args);

    /* Free the page from the memory. */

    pthread_join(pp_tid, NULL);

    /* Do not print \n%%%... on last page. */
    if (queue_size(stream_q) > 0)
      fprintf(output_stream, "%s", NEW_PAGE);

    free_page(page, lines);
  } while (queue_size(stream_q) > 0);

  assert(queue_size(stream_q) == 0); // Just in case (debug).
  free(stream_q);
}

/*
  Single thread execution. Same as the mthread_exec but simpler.
 */
void sthread_exec(FILE* input_stream, FILE* output_stream, const unsigned cols,
                  const unsigned lines, const unsigned width, const unsigned gap)
{
  Queue* stream_q = queue_create();
  load_words(stream_q, input_stream, width);

  const size_t page_width = (width * cols + (cols - 1) * gap) * 4;
  char** page;

  do {
    page = die_on_fail_palloc(lines, page_width);
    format_page(page, stream_q, cols, lines, width, gap);
    print_page(page, output_stream, lines);
    if (queue_size(stream_q) > 0)
      fprintf(output_stream, "%s", NEW_PAGE);
    free_page(page, lines);
  } while (queue_size(stream_q) > 0);

  assert(queue_size(stream_q) == 0);
  free(stream_q);
}
