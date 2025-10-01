#include "stack.h"

StackError stackInit(Stack* stk, size_t initialCapacity) {
    StackError stackError = NaE;
    if ((stackError = stackVerify(stk)) 
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
    if ((stackError = stackVerify(stk)))
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
    if ((stackError = stackVerify(stk))) {
        if (error) *error = stackError;
        return STACK_POISON;
    }
    
    size_t previousSize = stk->size;

    if (stk->size == 0) {
        if (error) *error = EmptyStackDataError;
        return STACK_POISON;
    }

    int value = stk->data[--stk->size];
    stk->data[stk->size] = 0;

    if (stk->size != previousSize - 1) {
        if (error) *error = DecrementationError;
        return value;
    }
    if (stk->data[previousSize] != 0) {
        if (error) *error = FailedPopError;
        return value;
    }

    if (error) *error = stackVerify(stk);
    return value;
}

// another way (reverse? ranges?)
StackError stackDestroy(Stack** stkPtr) {
    if (!stkPtr) return NaE; //smth else
    
    StackError stackError = NaE;
    Stack* stk = *stkPtr;
    stackError = stackVerify(stk);
    switch (stackError) {
        case NaE:
        case NullDataPointerError:
        case InvalidCapacityError:
        case SizeOutOfBoundsError: break;
        default: return stackError;
    }

    if (*stk->data) {
        for (size_t i = 0; i < stk->capacity; i++)
            stk->data[i] = STACK_POISON;
        free(stk->data);
        stk->data = NULL;
    }

    stk->size = 0;
    stk->capacity = 0;
    stk->isInitialized = false;
    stk->isDestroyed = true;

    free(stkPtr);

    return NaE;
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