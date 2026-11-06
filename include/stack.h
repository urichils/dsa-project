#ifndef STACK_H
#define STACK_H
#define STACK_CAPACITY 64

typedef struct {
    char actions[STACK_CAPACITY][96];
    int top;
} ActionStack;

void stack_init(ActionStack *stack);
int stack_push(ActionStack *stack, const char *action);
int stack_pop(ActionStack *stack, char *out, int out_size);
void stack_print(const ActionStack *stack);

#endif
