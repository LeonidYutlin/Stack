#ifndef STACK_H
#define STACK_H

//#define verifiedReturn(A) if(!A) return A; ???

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
    ReinitializationError       = -200,
    MemoryAllocationError       = -201,
    InvalidInitialCapacityError = -202,
    IncrementationError         = -300,
    TransmutedValueError        = -301,
    UnidentifiedStackError      = -900,
};

StackError stackInit(Stack* stk, size_t initialCapacity);
StackError stackPush(Stack* stk, int value);

#endif STACK_H