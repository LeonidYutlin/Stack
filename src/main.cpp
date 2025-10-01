#include <stdio.h>

//#define STACK_POISON -1
#include "stack/stack.h"

int main() {
    Stack stk = {};
    printf("Error code: %d\n", stackInit(&stk, 5));
    printf("Error code: %d\n", stackPush(&stk, 10));
    printf("Error code: %d\n", stackPush(&stk, 5));
    printf("Error code: %d\n", stackPush(&stk, -4));
    printf("Error code: %d\n", stackPush(&stk, 6));

    StackError error = NaE;

    printf("current index: %ld out of %ld total elements\n", stk.size, stk.capacity);

    printf("Current element in stack: %d\n", stackPop(&stk));
    printf("Error code: %d\n", error);
    printf("Current element in stack: %d\n", stackPop(&stk, &error));
    printf("Error code: %d\n", error);
    printf("Current element in stack: %d\n", stackPop(&stk, &error));
    printf("Error code: %d\n", error);
    printf("Current element in stack: %d\n", stackPop(&stk, &error));
    printf("Error code: %d\n", error);
    printf("Current element in stack: %d\n", stackPop(&stk, &error));
    printf("Error code: %d\n", error);

    return 0;
}