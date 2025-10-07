#include <stdio.h>

#include "stack/stack.h"

int main() {

    FILE* dumpyard = fopen("logs/dumpyard.txt", "w");

    int stkID = stackInit(5);
    if (stkID < 0) {
        printf("Cant init stack, error code: %d", stkID);
    }
    printf("Error code: %d\n", stackPush(stkID, 10));
    printf("Error code: %d\n", stackPush(stkID, 5));
    printf("Error code: %d\n", stackPush(stkID, -4));
    printf("Error code: %d\n", stackPush(stkID, 5));
    printf("Error code: %d\n", stackPush(stkID, -4));

    stackDump(dumpyard, stkID);


    printf("Error code: %d\n", stackPush(stkID, 6));
    printf("Error code: %d\n", stackPush(stkID, 9));
    printf("Error code: %d\n", stackPush(stkID, 8));
    printf("Error code: %d\n", stackPush(stkID, 8));
    printf("Error code: %d\n", stackPush(stkID, 8));

    stackDump(dumpyard, stkID);

    printf("Error code: %d\n", stackPush(stkID, 8));
    printf("Error code: %d\n", stackPush(stkID, 8));
    printf("Error code: %d\n", stackPush(stkID, 8));

    stackDump(dumpyard, stkID);

    StackError error = NaE;

    printf("current index: %lu out of %lu total elements\n", stackGetSize(stkID), stackGetCapacity(stkID));

    printf("Current element in stack: %d\n", stackPop(stkID));
    printf("Error code: %d\n", error);
    stackDump(dumpyard, stkID);
    printf("Current element in stack: %d\n", stackPop(stkID, &error));
    printf("Error code: %d\n", error);
    stackDump(dumpyard, stkID);
    // printf("Current element in stack: %d\n", stackPop(&stk, &error));
    // printf("Error code: %d\n", error);
    // stackDump(stderr, &stk);
    // printf("Current element in stack: %d\n", stackPop(&stk, &error));
    // printf("Error code: %d\n", error);
    stackDump(dumpyard, stkID);

    // stk.size = 0;
    // stk.data[0] = 100;
    // stk.data[1] = 101; Cant do that anymore! HAHA!
    stackDump(dumpyard, stkID);

    // Stack* stkPtr = stkID;
    stackDestroy(stkID);
    stackDump(dumpyard, stkID);

    stackDump(dumpyard, -1);

    int stkID2 = stackInit(5);
    if (stkID2 < 0) {
        printf("Cant init stack, error code: %d", stkID);
    }
    printf("Error code: %d\n", stackPush(stkID2, 10));
    printf("Error code: %d\n", stackPush(stkID2, 5));
    printf("Error code: %d\n", stackPush(stkID2, -4));

    stackDump(dumpyard, stkID2);
    stackDestroy(stkID2);

    //printf("Error code: %d\n", stackPush(&stk, 10));
    //printf("Error code: %d\n", error);
    fclose(dumpyard);

    int a = stackInit(3);
    int b = stackInit(3);
    int c = stackInit(3);
    printf("Stack Initialized with an ID: %d\n", a);
    printf("Stack Initialized with an ID: %d\n", b);
    printf("Stack Initialized with an ID: %d\n", c);

    stackDestroy(b);

    int d = stackInit(3);
    int d1 = stackInit(3);
    int d2 = stackInit(3);
    int d3 = stackInit(3);
    int d4 = stackInit(3);
    printf("Stack Initialized with an ID: %d\n", d);
    printf("Stack Initialized with an ID: %d\n", d1);
    printf("Stack Initialized with an ID: %d\n", d2);
    printf("Stack Initialized with an ID: %d\n", d3);
    printf("Stack Initialized with an ID: %d\n", d4);

    stackDestroy(a);
    stackDestroy(c);
    stackDestroy(d);
    stackDestroy(d1);
    stackDestroy(d2);
    stackDestroy(d3);
    stackDestroy(d4);

    return 0;
}