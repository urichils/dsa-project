#ifndef QUEUE_H
#define QUEUE_H
#define QUEUE_CAPACITY 32

typedef struct {
    int data[QUEUE_CAPACITY];
    int front;
    int rear;
    int size;
} TurnQueue;

void queue_init(TurnQueue *queue);
int queue_enqueue(TurnQueue *queue, int value);
int queue_dequeue(TurnQueue *queue, int *value);
int queue_is_empty(const TurnQueue *queue);

#endif
