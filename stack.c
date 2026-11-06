#include <stdio.h>
#include <string.h>
#include "include/stack.h"
void stack_init(ActionStack *s){s->top=-1;}
int stack_push(ActionStack *s,const char *a){
    if(s->top>=STACK_CAPACITY-1)return 0;
    snprintf(s->actions[++s->top],sizeof(s->actions[0]),"%s",a);return 1;
}
int stack_pop(ActionStack *s,char *out,int size){
    if(s->top<0)return 0;
    snprintf(out,(size_t)size,"%s",s->actions[s->top--]);return 1;
}
void stack_print(const ActionStack *s){
    puts("\n=== BATTLE ACTION HISTORY (STACK, NEWEST FIRST) ===");
    if(s->top<0){puts("No actions yet.");return;}
    for(int i=s->top;i>=0;i--)printf("%d. %s\n",s->top-i+1,s->actions[i]);
}
