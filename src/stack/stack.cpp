#include "stack.h"

StackError stackInit(Stack* stk, size_t initialCapacity) {
    assert(stk);
    assert(initialCapacity);
    
    if (stk->isInitialized) return ReinitializationError;

    int* tempPtr = (int*) calloc(initialCapacity, sizeof(int));
    assert(tempPtr);
    if (!tempPtr) return MemoryAllocationError;
    stk->data = tempPtr;
    stk->size = 0;
    stk->capacity = initialCapacity;
    stk->isDestroyed = false;
    stk->isInitialized = true;
    /* 
    A way of doing all this via cpp's default struct values
    *stk = {.data = tempPtr, 
            .capacity = initialCapacity, 
            .isInitialized = true};
    */

    assert(stk->data);
    assert(stk->size == 0);
    assert(stk->capacity == initialCapacity);
    assert(!stk->isDestroyed);
    assert(stk->isInitialized);

    return NaE;
}