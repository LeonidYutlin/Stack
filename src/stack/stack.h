#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef struct Stack {
    int* data = NULL;
    size_t size = 0;
    size_t capacity = 0;
    bool isDestroyed = false;
    bool isInitialized = false;
    //char state = '\0' 'd' 'i' 'e'
} Stack;

/*
    0 is okay
    -1xx is generalError
    -2xx is initSpecificError
    -9xx is otherError
*/
enum StackError {
    NaE = 0, //NotAnError
    MemoryAllocationError = -101,
    ReinitializationError = -201,
    UnidentifiedStackError = -901,
    DestroyedStackError
};

StackError stackInit(Stack* stk, size_t initialCapacity);

#endif