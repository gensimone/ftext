#ifndef QUEUE_H
#define QUEUE_H

typedef struct Queue Queue;

Queue* queue_create();
int queue_size(const Queue* q);
void queue_free(Queue* q);
int queue_push(Queue* q, const char* str);
char* queue_pop(Queue* q);
char* queue_head(Queue* q);
char* queue_tail(Queue* q);

#endif
