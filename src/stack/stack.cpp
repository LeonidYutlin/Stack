#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "common/stackConst.h"

#ifdef _DEBUG

#undef  stackDump
#define stackDump(fileStream, stk) \
        stackDump(fileStream, stk, true, __FILE__, __LINE__);

#endif

static StackStatus stackCheckCanaries(Stack* stk);

StackStatus stackInit(Stack* stk, size_t initialCapacity){
    if (stackVerify(stk) != UninitializedStack)
        return AttemptedReinitialization;
    if (initialCapacity == 0)
        return BadInitialCapacity;
    
    size_t capacityWithCanaries = initialCapacity + CANARY_LEFT_COUNT 
                                  + CANARY_RIGHT_COUNT;

    StackUnit* tempPtr = (StackUnit*)calloc(capacityWithCanaries, sizeof(StackUnit));
    if (!tempPtr)
        return stk->status = FailMemoryAllocation;

    stk->data = tempPtr;
    stk->size = 0;
    stk->capacity = initialCapacity;
    stk->capacityWithCanaries = capacityWithCanaries;
    stk->status = OK;
    for (size_t i = 0; i < CANARY_LEFT_COUNT; i++)
        stk->data[i] = CANARY_LEFT;
    for (size_t i = 0; i < CANARY_RIGHT_COUNT; i++)
        stk->data[stk->capacityWithCanaries - 1 - i] = CANARY_RIGHT;

    return stackVerify(stk);
}

Stack* stackInit(size_t initialCapacity, StackStatus* status) {
    if (initialCapacity == 0) {
        if (status)
            *status = BadInitialCapacity;
        return NULL;
    }

    Stack* stk = (Stack*)calloc(1, sizeof(Stack));
    if (!stk) {
        if (status)
            *status = FailMemoryAllocation;
        return NULL;
    }

    if (stackInit(stk, initialCapacity)) {
        if (status)
            *status = FailMemoryAllocation;
        free(stk);
        return NULL;
    }

    if (status)
        *status = stackVerify(stk);
    return stk;
}

StackStatus stackPush(Stack* stk, StackUnit value){
    StackStatus stackStatus = stackVerify(stk);
    if (stackStatus)
        return stackStatus;

    if (stk->size == stk->capacity)
        if ((stackStatus = stackExpandCapacity(stk, stk->capacity)))
            return stackStatus;
    
    size_t previousSize = stk->size;
    stk->data[CANARY_LEFT_COUNT + stk->size++] = value;
    if (stk->size != previousSize + 1)
        return stk->status = FailIncrement;
    if (stk->data[CANARY_LEFT_COUNT + previousSize] != value)
        return stk->status = PushedValueTransmuted;

    return stackVerify(stk);
}

StackUnit stackPop(Stack* stk, StackStatus* status){
    StackStatus stackStatus = stackVerify(stk);
    if (stackStatus) {
        if (status)
            *status = stk->status;
        return STACK_POISON;
    }

    if (stk->size == 0) {
        stk->status = NoValueToPop;
        if (status)
            *status = stk->status;
        return STACK_POISON;
    }

    size_t previousSize = stk->size;
    StackUnit value = stk->data[CANARY_LEFT_COUNT + (--stk->size)];
    stk->data[CANARY_LEFT_COUNT + stk->size] = 0;

    if (stk->size != previousSize - 1) {
        stk->status = FailDecrement;
        if (status)
            *status = stk->status;
        return value;
    }
    if (stk->data[CANARY_LEFT_COUNT + previousSize] != 0) {
        stk->status = FailPop;
        if (status)
            *status = stk->status;
        return value;
    }

    if (status)
        *status = stackVerify(stk);
    return value;
}

StackStatus stackDestroy(Stack* stk, bool isAlloced){
    if (!stk)
        return NullStackPointer;

    if (stk->data) {
        for (size_t i = 0; i < stk->capacityWithCanaries; i++)
            stk->data[i] = STACK_POISON;
        free(stk->data);
        stk->data = NULL;
    }

    stk->size = 0;
    stk->capacity = 0;
    stk->status = DestroyedStack;

    if (isAlloced)
        free(stk);

    return OK;
}

StackStatus stackVerify(Stack* stk){
    if (!stk)
        return NullStackPointer;
    if (stk->status)
        return stk->status;
    if (!stk->data)
        return stk->status = NullDataPointer;
    if (stk->capacity == 0)
        return stk->status = BadCapacity;
    if (stk->size > stk->capacity)
        return stk->status = BadSize;
    StackStatus status = stackCheckCanaries(stk);
    if (status != OK) 
        return stk->status = status;
    return stk->status;
}

StackStatus stackCheckCanaries(Stack* stk) {
    for (size_t i = 0; i < CANARY_LEFT_COUNT; i++)
        if (stk->data[i] != CANARY_LEFT) 
            return CorruptedCanary;
    for (size_t i = 0; i < CANARY_RIGHT_COUNT; i++)
        if (stk->data[stk->capacityWithCanaries - 1 - i] != CANARY_RIGHT) 
            return CorruptedCanary;
    return OK;
}


StackStatus stackExpandCapacity(Stack* stk, size_t additionalCapacity) {
    StackStatus stackStatus = stackVerify(stk);
    if (stackStatus)
        return stackStatus;

    StackUnit *tempPtr = (StackUnit*)realloc(stk->data,
                                                 (stk->capacityWithCanaries 
                                                    + additionalCapacity)
                                                 * sizeof(StackUnit));
    if (!tempPtr)
        return stk->status = FailMemoryReallocation;
    memset(tempPtr + CANARY_LEFT_COUNT + stk->capacity, 0, 
           (additionalCapacity + CANARY_RIGHT_COUNT) * sizeof(StackUnit));
    stk->capacityWithCanaries += additionalCapacity;
    stk->capacity += additionalCapacity;
    stk->data = tempPtr;
    for (size_t i = 0; i < CANARY_RIGHT_COUNT; i++)
        stk->data[stk->capacityWithCanaries - 1 - i] = CANARY_RIGHT;
    
    return OK;
}