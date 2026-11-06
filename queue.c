#include "include/queue.h"
void queue_init(TurnQueue *q){q->front=0;q->rear=0;q->size=0;}
int queue_enqueue(TurnQueue *q,int v){
    if(q->size==QUEUE_CAPACITY)return 0;
    q->data[q->rear]=v;q->rear=(q->rear+1)%QUEUE_CAPACITY;q->size++;return 1;
}
int queue_dequeue(TurnQueue *q,int *v){
    if(!q->size)return 0;
    *v=q->data[q->front];q->front=(q->front+1)%QUEUE_CAPACITY;q->size--;return 1;
}
int queue_is_empty(const TurnQueue *q){return q->size==0;}
