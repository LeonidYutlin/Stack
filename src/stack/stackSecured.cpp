#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "stack.h"
#include "stackSecured.h"

static const size_t DEFAULT_INITIAL_STACK_MANAGER_CAPACITY = 5;
typedef struct StackManager {
    size_t size     = 0;
    size_t capacity = 0;
    Stack** data = NULL;
} StackManager;
static StackManager STACK_MANAGER = {0};

static StackStatus allocStackManagerData();
static void freeEmptyStackManager();
static Stack* getStack(Stack_t stkID);
static int findFreeStackId();

#ifdef _DEBUG

#undef  stackDump
#define stackDump(fileStream, stk) \
        stackDump(fileStream, stk, true, __FILE__, __LINE__);

#endif

Stack_t stackInit(size_t initialCapacity, StackStatus* statusPtr) {
    if (initialCapacity == 0) {
        if (statusPtr)
            *statusPtr = BadInitialCapacity;
        return -1;
    }

    StackStatus status = allocStackManagerData();
    if (status) {
        if (statusPtr)
            *statusPtr = status;
        return -1;
    }
    Stack* stk = stackDynamicInit(initialCapacity, &status);
    if (status) {
        if (statusPtr)
            *statusPtr = status;
        freeEmptyStackManager();
        return -1;
    }
    Stack_t stkID = findFreeStackId();
    STACK_MANAGER.data[stkID] = stk;
    STACK_MANAGER.size++;

    status = stackVerify(stkID);
    if (statusPtr)
            *statusPtr = status;
    return status == OK ? stkID : -1;
}

StackStatus stackPush(Stack_t stkID, StackUnit value) {
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;

    return stackPush(stk, value);
}

StackUnit stackPop(Stack_t stkID, StackStatus *status) {
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;

    return stackPop(stk, status);
}

StackStatus stackDestroy(Stack_t stkID) {
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;

    stackDestroy(stk, true);
    STACK_MANAGER.data[stkID] = NULL;
    STACK_MANAGER.size--;
    freeEmptyStackManager();

    return OK;
}

StackStatus stackVerify(Stack_t stkID) {
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;
    return stk->status = stackVerify(stk);
}

StackStatus stackExpandCapacity(Stack_t stkID, size_t additionalCapacity) {
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;

    return stackExpandCapacity(stk, additionalCapacity);
}

size_t stackGetSize(Stack_t stkID, StackStatus *status) {
    Stack* stk = getStack(stkID);
    if (!stk) {
        if (status)
            *status = InvalidStackID;
        return 0;
    }

    return stk->size;
}

size_t stackGetCapacity(Stack_t stkID, StackStatus* status) {
    Stack* stk = getStack(stkID);
    if (!stk) {
        if (status)
            *status = InvalidStackID;
        return 0;
    }

    return stk->capacity;
}

// StackStatus stackGetError(int stkID) {
//     Stack* stk = getStack(stkID);
//     if (!stk)
//         return InvalidStackID;

//     return stk->status;
// }

StackStatus allocStackManagerData() {
    if (!STACK_MANAGER.data) {
        Stack** tempPtr = (Stack**)calloc(DEFAULT_INITIAL_STACK_MANAGER_CAPACITY,
                                          sizeof(Stack*));
        if (!tempPtr)
            return FailMemoryAllocation;

        STACK_MANAGER.capacity = DEFAULT_INITIAL_STACK_MANAGER_CAPACITY;
        STACK_MANAGER.size = 0;
        STACK_MANAGER.data = tempPtr;
    } else if (STACK_MANAGER.size == STACK_MANAGER.capacity) {
        Stack** tempPtr = (Stack**)realloc(STACK_MANAGER.data,
                                           2 * STACK_MANAGER.capacity
                                           * sizeof(Stack*));
        if (!tempPtr)
            return FailMemoryReallocation;

        STACK_MANAGER.capacity *= 2;
        STACK_MANAGER.data = tempPtr;
    }

    return OK;
}

void freeEmptyStackManager() {
    if (STACK_MANAGER.size == 0) {
        free(STACK_MANAGER.data);
        STACK_MANAGER.data = NULL;
    }
}

Stack* getStack(Stack_t stkID) {
    return stkID < 0 || (unsigned int)stkID >= STACK_MANAGER.size
           ? NULL
           : STACK_MANAGER.data[stkID];
}

/*
    I know that a cast to (int) loses info but I need it to be an int and I dont think
    someone is making a StackManager that is so large it cannot cope
    with unsigned long being cast to int
*/
int findFreeStackId() {
    int stkID = -1;
    for (int i = 0; i < (int)STACK_MANAGER.size; i++) {
        if (STACK_MANAGER.data[i] == NULL) {
            stkID = i;
            break;
        }
    }

    return stkID == -1 ? (int)STACK_MANAGER.size : stkID;
}

#ifdef _DEBUG

#undef stackDump
void stackDumpSecured(FILE* fileStream, Stack_t stkID,
                      const char* fileName, int line) {
    assert(fileStream);

    return stackDump(fileStream, getStack(stkID), false, fileName, line);
}

#endif
