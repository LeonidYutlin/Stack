#include <stdio.h>

#include "stack/stack.h"

int main() {
    FILE* dumpyard = fopen("../.log/dumpyard.txt", "w");
    if (dumpyard == NULL) {
        puts("Error opening file!");
         return -1;
    }

    Stack stk = {};
    Stack* stkAlloced = stackInit(10);
    stackInit(&stk, 10);
    stackDump(dumpyard, &stk);

    stackPush(stkAlloced, 5);
    stackPush(stkAlloced, 2);
    stackPush(stkAlloced, -1);
    stackPush(&stk, 7);

    stackDump(dumpyard, stkAlloced);

    stackExpandCapacity(&stk, 23);

    stackDump(dumpyard, &stk);

    stackDestroy(&stk);
    stackDestroy(stkAlloced, true);

    // Stack_t stk = stackInit(5);
    // if (stk < 0) {
    //     printf("Cant init stack, status code: %d", stk);
    // }
    // printf("Error code: %d\n", stackPush(stk, 10));
    // printf("Error code: %d\n", stackPush(stk, 5));
    // printf("Error code: %d\n", stackPush(stk, -4));
    // printf("Error code: %d\n", stackPush(stk, 5));
    // printf("Error code: %d\n", stackPush(stk, -4));

    // stackDump(dumpyard, stk);


    // printf("Error code: %d\n", stackPush(stk, 6));
    // printf("Error code: %d\n", stackPush(stk, 9));
    // printf("Error code: %d\n", stackPush(stk, 8));
    // printf("Error code: %d\n", stackPush(stk, 8));
    // printf("Error code: %d\n", stackPush(stk, 8));

    // stackDump(dumpyard, stk);

    // printf("Error code: %d\n", stackPush(stk, 8));
    // printf("Error code: %d\n", stackPush(stk, 8));
    // printf("Error code: %d\n", stackPush(stk, 8));

    // stackDump(dumpyard, stk);

    // StackStatus status = NaE;

    // printf("current index: %lu out of %lu total elements\n", stackGetSize(stk), stackGetCapacity(stk));

    // printf("Current element in stack: %d\n", stackPop(stk));
    // printf("Error code: %d\n", status);
    // stackDump(dumpyard, stk);
    // printf("Current element in stack: %d\n", stackPop(stk, &status));
    // printf("Error code: %d\n", status);
    // stackDump(dumpyard, stk);

    // stackDump(dumpyard, stk);

    // stackDestroy(stk);
    // stackDump(dumpyard, stk);

    // stackDump(dumpyard, -1);

    // Stack_t stk2 = stackInit(5);
    // if (stk2 < 0) {
    //     printf("Cant init stack, status code: %d", stk);
    // }
    // printf("Error code: %d\n", stackPush(stk2, 10));
    // printf("Error code: %d\n", stackPush(stk2, 5));
    // printf("Error code: %d\n", stackPush(stk2, -4));

    // stackDump(dumpyard, stk2);
    // stackDestroy(stk2);

    // Stack_t a = stackInit(3);
    // Stack_t b = stackInit(3);
    // Stack_t c = stackInit(3);
    // printf("Stack Initialized with an ID: %d\n", a);
    // printf("Stack Initialized with an ID: %d\n", b);
    // printf("Stack Initialized with an ID: %d\n", c);

    // stackDestroy(b);

    // Stack_t d = stackInit(3);
    // Stack_t d1 = stackInit(3);
    // Stack_t d2 = stackInit(3);
    // Stack_t d3 = stackInit(3);
    // Stack_t d4 = stackInit(3);
    // stackDump(dumpyard, d4);
    // printf("Stack Initialized with an ID: %d\n", d);
    // printf("Stack Initialized with an ID: %d\n", d1);
    // printf("Stack Initialized with an ID: %d\n", d2);
    // printf("Stack Initialized with an ID: %d\n", d3);
    // printf("Stack Initialized with an ID: %d\n", d4);

    // stackDestroy(a);
    // stackDestroy(c);
    // stackDestroy(d);
    // stackDestroy(d1);
    // stackDestroy(d2);
    // stackDestroy(d3);
    // stackDestroy(d4);

    fclose(dumpyard);
    return 0;
}