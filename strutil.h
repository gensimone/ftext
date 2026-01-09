#include "queue.h"
#include <stddef.h>

unsigned int rstrlen(const char* str);
char* strspace(const size_t n);
void sx_align(Queue* q, char* dst);
void bx_align(Queue* q, char* dst, int nspaces);
