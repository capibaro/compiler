#include <stdlib.h>
#include <string.h>
#include "stack.h"

Stack *InitStack() {
    Stack *sta = (Stack *)malloc(sizeof(Stack));
    if (sta == NULL) exit(1);
    sta->top = NULL;
    sta->size = 0;
    return sta;
}

int isEmpty(Stack *sta) {
    return (sta->size == 0) ? 1 : 0;
}

void Push(Stack *sta, int state, char str[]) {
    StackNode *now = (StackNode *)malloc(sizeof(StackNode));
    if (now == NULL) exit(1);
    now->state = state;
    memmove(now->str, str, strlen(str) + 1);
    now->prev = NULL;
    if (sta->top == NULL) {
        sta->top = now;
        sta->size++;
    } else {
        now->prev = sta->top;
        sta->top = now;
        sta->size++;
    }
}

void Pop(Stack *sta) {
    StackNode *p = NULL;
    p = sta->top;
    sta->top = sta->top->prev;
    free(p);
    sta->size--;
}