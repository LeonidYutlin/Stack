#include "stack.h"

StackError stackInit(Stack* stk, size_t initialCapacity) {
    StackError stackError = NaE;
    if (!(stackError = stackVerify(stk)) 
        && stackError != UninitializedStackError)
            return stackError;
    
    if (stk->isInitialized)
        return ReinitializationError;

    int* tempPtr = (int*) calloc(initialCapacity, sizeof(int));
    if (!tempPtr) 
        return MemoryAllocationError;
    
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

    /*
    if (!(stackError = stackVerify(stk)))
        return stackError;

    return stackError;
    */
    
    return stackVerify(stk);
}

StackError stackPush(Stack* stk, int value) {
    StackError stackError = NaE;
    if (!(stackError = stackVerify(stk)))
        return stackError;
    
    size_t previousSize = stk->size;

    stk->data[stk->size++] = value;

    if (stk->size != previousSize + 1)
        return IncrementationError;
    if (stk->data[previousSize] != value)
        return TransmutedValueError;

    return stackVerify(stk);
}

int stackPop(Stack* stk, StackError* error) {
    StackError stackError = NaE;
    if (!(stackError = stackVerify(stk)))
        return stackError;
    
    size_t previousSize = stk->size;

    int value = stk->data[stk->size];
    stk->data[stk->size] = 0;
    stk->size--;

    if (stk->size != previousSize - 1)
        return DecrementationError;
    if (stk->data[previousSize] != 0)
        return FailedPoisonError;

    return stackVerify(stk);
}

StackError stackVerify(Stack* stk) {
    if (!stk) 
        return NullStackPointerError;
    if (stk->isDestroyed) 
        return DestroyedStackError;
    if (!stk->isInitialized) 
        return UninitializedStackError;
    if (!stk->data) 
        return NullDataPointerError;
    if (stk->capacity == 0)
        return InvalidCapacityError;
    if (stk->size >= stk->capacity)
        return SizeOutOfBoundsError;
    return NaE;
}