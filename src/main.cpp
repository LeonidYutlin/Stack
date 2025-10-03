#include <stdio.h>

//#define STACK_POISON -1
#include "stack/stack.h"

int main() {

    //perrors ?

    FILE* dumpyard = fopen("logs/dumpyard.txt", "w");

    Stack stk = {};
    printf("Error code: %d\n", stackInit(&stk, 5));
    printf("Error code: %d\n", stackPush(&stk, 10));
    printf("Error code: %d\n", stackPush(&stk, 5));
    printf("Error code: %d\n", stackPush(&stk, -4));

    stackDump(dumpyard, &stk);


    printf("Error code: %d\n", stackPush(&stk, 6));
    printf("Error code: %d\n", stackPush(&stk, 9));
    printf("Error code: %d\n", stackPush(&stk, 8));
    printf("Error code: %d\n", stackPush(&stk, 8));
    printf("Error code: %d\n", stackPush(&stk, 8));

    stackDump(dumpyard, &stk);
    
    printf("Error code: %d\n", stackPush(&stk, 8));
    printf("Error code: %d\n", stackPush(&stk, 8));
    printf("Error code: %d\n", stackPush(&stk, 8));

    stackDump(dumpyard, &stk);

    StackError error = NaE;

    printf("current index: %lu out of %lu total elements\n", stk.size, stk.capacity);

    printf("Current element in stack: %d\n", stackPop(&stk));
    printf("Error code: %d\n", error);
    stackDump(dumpyard, &stk);
    printf("Current element in stack: %d\n", stackPop(&stk, &error));
    printf("Error code: %d\n", error);
    stackDump(dumpyard, &stk);
    // printf("Current element in stack: %d\n", stackPop(&stk, &error));
    // printf("Error code: %d\n", error);
    // stackDump(stderr, &stk);
    // printf("Current element in stack: %d\n", stackPop(&stk, &error));
    // printf("Error code: %d\n", error);
    stackDump(dumpyard, &stk);

    stk.size = 0;
    stk.data[0] = 100;
    stk.data[1] = 101;
    stackDump(dumpyard, &stk);

    Stack* stkPtr = &stk;
    stackDestroyPtr(&stkPtr);
    stackDestroy(&stk); //excessive, but the sanitizer does not think so
    stackDump(dumpyard, stkPtr);

    stackDump(dumpyard, NULL);

    Stack stk2 = {};
    printf("Error code: %d\n", stackInit(&stk2, 5));
    printf("Error code: %d\n", stackPush(&stk2, 10));
    printf("Error code: %d\n", stackPush(&stk2, 5));
    printf("Error code: %d\n", stackPush(&stk2, -4));
    printf("Error code: %d\n", stackPush(&stk2, STACK_POISON));

    stackDump(dumpyard, &stk2);
    stackDestroy(&stk2);

    //printf("Error code: %d\n", stackPush(&stk, 10));
    //printf("Error code: %d\n", error);
    fclose(dumpyard);

    return 0;
}