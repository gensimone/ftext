#ifndef QUEUE_H
#define QUEUE_H

typedef struct Queue Queue;

Queue* queue_create();
void queue_destroy(Queue* q);
int queue_push(Queue* q, const char* str);
char* queue_pop(Queue* q);
unsigned int queue_length(const Queue* q);
unsigned int queue_size(const Queue* q);
char* queue_head(const Queue* q);
char* queue_tail(const Queue* q);

#endif
