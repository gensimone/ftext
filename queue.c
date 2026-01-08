#include "queue.h"
#include "alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
  struct Node* next;
  struct Node* prev;
  char* value;
} Node;

struct Queue {
  int size;
  Node* tail;
  Node* head;
};

Queue* queue_create()
{
  Queue* q = (Queue*) die_on_fail_malloc(sizeof(Queue));

  q->size = 0;
  q->tail = NULL;
  q->head = NULL;

  return q;
}

int queue_size(const Queue* q)
{
  return q->size;
}

void queue_free(Queue* q)
{
  Node* n;
  while ((n = q->tail) != NULL) {
    q->tail = n->next;
    free(n);
  }

  q->head = NULL;
  q->size = 0;
}

int queue_push(Queue* q, const char* str)
{
  Node* n = (Node*) die_on_fail_malloc(sizeof(Node));

  n->next = q->tail;
  n->prev = NULL;
  n->value = strdup(str);

  if (q->size == 0)
    q->head = n;

  if (q->tail != NULL)
    q->tail->prev = n;

  q->tail = n;
  q->size++;

  return 0;
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
  q->size--;

  return out;
}

char* queue_head(Queue* q)
{
  return q->head->value;
}

char* queue_tail(Queue* q)
{
  return q->tail->value;
}
