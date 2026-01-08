#include "util.h"
#include <stdlib.h>

/* Same as calloc but die if the calloc fails
   to allocate the requested memory.
*/
void* die_on_fail_calloc(size_t nmemb, size_t size)
{
  void* ptr = calloc(nmemb, size);
  if (ptr == NULL)
    die("cannot calloc %zu bytes", size);
  return ptr;
}

/* Same as calloc but die if the calloc fails
   to allocate the requested memory.
*/
void* die_on_fail_malloc(size_t size)
{
  void* ptr = malloc(size);
  if (ptr == NULL)
    die("cannot malloc %zu bytes", size);
  return ptr;
}

/* Same as realloc but die if the realloc fails
   to allocate the requested memory.
*/
void* die_on_fail_realloc(void* ptr, size_t size)
{
  void* rptr = realloc(ptr, size);
  if (rptr == NULL)
    die("cannot realloc %zu bytes", size);
  return rptr;
}

/*
  Allocate a pointer (page) to nl pointers of type char of size nc.
  Use calloc to allocate the pointers.
  Invoke 'die' if calloc fails.
 */
char** die_on_fail_palloc(int nl, int nc)
{
  char** pp = (char**) die_on_fail_calloc(nl, sizeof(char*));
  for (int i = 0; i < nl; i++)
    pp[i] = die_on_fail_calloc(nc, sizeof(char));
  return pp;
}

/*
  Free the pointer pp and all its n pointers associated.
  The pointer pp must have been returned by a previous call
  to die_on_fail_palloc or a similar function.
 */
void free_page(char** pp, int n)
{
  for (int i = 0; i < n; i++)
    free(pp[i]);
  free(pp);
}
