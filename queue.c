#include "queue.h"
#include "alloc.h"
#include "strutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
  struct Node* next;
  struct Node* prev;
  char* value;
} Node;

struct Queue {
  unsigned int size;
  unsigned int length;
  Node* tail;
  Node* head;
};

Queue* queue_create()
{
  Queue* q = (Queue*) die_on_fail_malloc(sizeof(Queue));

  q->size = 0;
  q->length = 0;
  q->tail = NULL;
  q->head = NULL;

  return q;
}

void queue_destroy(Queue* q)
{
  Node* n;
  while ((n = q->tail) != NULL) {
    q->tail = n->next;
    free(n->value);
    free(n);
  }

  q->head = NULL;
  q->size = 0;
  q->length = 0;
}

void queue_push(Queue* q, char* str)
{
  Node* n = (Node*) die_on_fail_malloc(sizeof(Node));

  n->next = q->tail;
  n->prev = NULL;
  n->value = str;

  if (q->size == 0)
    q->head = n;

  if (q->tail != NULL)
    q->tail->prev = n;

  q->tail = n;
  q->size++;
  q->length += rstrlen(str);
}

void queue_top(Queue* q, char* str)
{
  Node* n = (Node*) die_on_fail_malloc(sizeof(Node));

  n->next = NULL;
  n->prev = q->head;
  n->value = str;

  if (q->size == 0)
    q->tail = n;

  if (q->head != NULL)
    q->head->next = n;

  q->head = n;
  q->size++;
  q->length += rstrlen(str);
}

char* queue_pop(Queue* q)
{
  if (q->size == 0)
    return NULL;

  char* out = q->head->value;

  Node* old_head = q->head;
  if (q->size > 1) {
    old_head->prev->next = NULL;
    q->head = old_head->prev;
  } else if (q->size == 1) {
    q->tail = NULL;
    q->head = NULL;
  }

  free(old_head);

  q->length -= rstrlen(out);
  q->size--;

  return out;
}

unsigned int queue_length(const Queue* q)
{
  return q->length;
}

unsigned int queue_size(const Queue* q)
{
  return q->size;
}

char* queue_head(const Queue* q)
{
  return q->head ? q->head->value : NULL;
}

char* queue_tail(const Queue* q)
{
  return q->tail ? q->tail->value : NULL;
}
