#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <limits.h>

typedef int StackUnit;

/*
    0 is okay
    -1xx is genericError
    -2xx is initSpecificError
    -3xx is pushSpecificError
    -9xx is otherError
*/
enum StackError {
    NaE                         =    0, //NotAnError
    NullStackPointerError       = -100,
    NullDataPointerError        = -101,
    DestroyedStackError         = -102,
    UninitializedStackError     = -103,
    InvalidCapacityError        = -104,
    SizeOutOfBoundsError        = -105,
    CorruptedCanaryError        = -106,
    ReinitializationError       = -200,
    MemoryAllocationError       = -201,
    MemoryReallocationError     = -202,
    InvalidInitialCapacityError = -203,
    IncrementationError         = -300,
    TransmutedValueError        = -301,
    DecrementationError         = -400,
    FailedPopError              = -401,
    EmptyStackDataError         = -402,
    UnidentifiedStackError      = -900
};

typedef struct Stack {
    StackUnit* data = NULL;
    size_t size = 0;
    size_t capacity = 0;
    size_t trueCapacity = 0;
    StackError error = UninitializedStackError;
    bool isDestroyed = false;
    bool isInitialized = false;
    //char state = '\0' 'd' 'i' 'e'
} Stack;

StackError stackInit(Stack* stk, size_t initialCapacity);
StackError stackPush(Stack* stk, StackUnit value);
StackUnit stackPop(Stack* stk, StackError* error = NULL);
StackError stackVerify(Stack* stk);
StackError stackDestroy(Stack* stk);

void stackDump(FILE* fileStream, Stack* stk, const char* fileName, int line);
#define stackDump(fileStream, stk) stackDump(fileStream, stk, __FILE__, __LINE__);

#endif