#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "stackSecured.h"

#undef stackDump

static const StackUnit STACK_POISON = 0xAbD06;

static const StackUnit CANARY_LEFT = 0xDEDDEDED;
static const StackUnit CANARY_RIGHT = 0xAAEDAEDA;

static const size_t CANARY_LEFT_COUNT = 1;
static const size_t CANARY_RIGHT_COUNT = 1;

static const char* QUOTES[] = {
    "Canary Canary Do Thy Hear Me ?",
    "SegFault? Not on my watch, bucko",
    "Aaaaand what has went wrong now?",
    "None of these words are in the Bible",
    "Et tu, Brute?",
    "Appledog",
    "Is my stack gonna make it? Prolly not",
    "OUR FOOD KEEPS BLOWING U-",
    "Now with 800% more poison!",
    "One must imagine stackDump happy...",
    "Boy oh boy I sure hope nothing bad has happened to my stack!",
    "I ATE IT ALL",
    "Ouch! That hurt!",
    "stackDump soboleznuet",
    "Tut mogla bit' vasha oshibka",
    "So what are you gonna say at my funeral now that you've killed me",
    "Fishy Fiesta is real",
    "Sunshine Banishes The Dark!"
};

typedef struct Stack {
    StackUnit* data = NULL;
    size_t size = 0;
    size_t capacity = 0;
    size_t capacityWithCanaries = 0;
    StackStatus status = InvalidStackID;
    bool isDestroyed = false;
    bool isInitialized = false;
} Stack;

static const size_t INITIAL_STACK_MANAGER_CAPACITY = 5;
typedef struct StackManager {
    size_t size = 0;
    size_t capacity = 0;
    Stack** data = NULL;
} StackManager;
static StackManager STACK_MANAGER = {0};

static Stack* getStack(int stkID);
static StackStatus stackCheckCanaries(Stack* stk);
static int printFormattedStackUnitString(FILE* fileStream, 
                                         Stack* stk, size_t index);

static void stackDumpInternal(FILE* fileStream, int stkID, bool isAdvanced, 
                              const char* fileName, int line);

#ifdef _DEBUG

#define stackDumpInternal(fileStream, stkID, isAdvanced) \
    stackDumpInternal(fileStream, stkID, isAdvanced, __FILE__, __LINE__);

#else

#define stackDumpInternal(fileStream, stkID, isAdvanced) ;

#endif

int stackInit(size_t initialCapacity){
    if (initialCapacity == 0) 
        return InvalidInitialCapacityError;

    if (!STACK_MANAGER.data) {
        Stack** tempPtr = (Stack**)calloc(INITIAL_STACK_MANAGER_CAPACITY, 
                                          sizeof(Stack*));
        if (!tempPtr)
            return MemoryAllocationError;

        STACK_MANAGER.capacity = INITIAL_STACK_MANAGER_CAPACITY;
        STACK_MANAGER.size = 0;
        STACK_MANAGER.data = tempPtr;
    } else if (STACK_MANAGER.size == STACK_MANAGER.capacity) {
        Stack** tempPtr = (Stack**)realloc(STACK_MANAGER.data, 
                                           2 * STACK_MANAGER.capacity 
                                           * sizeof(Stack*));
        if (!tempPtr)
            return MemoryReallocationError;
            
        STACK_MANAGER.capacity *= 2;
        STACK_MANAGER.data = tempPtr;
    }

    Stack* stk = (Stack*)calloc(1, sizeof(Stack));
    if (!stk)
        return MemoryAllocationError;
    int stkID = -1;
    for (int i = 0; i < (int)STACK_MANAGER.size; i++) {
        if (STACK_MANAGER.data[i] == NULL) {
            stkID = i;
            break;
        }
    }
    if (stkID == -1) 
        stkID = (int)STACK_MANAGER.size;
    STACK_MANAGER.data[stkID] = stk;
    STACK_MANAGER.size++;

    size_t capacityWithCanaries = initialCapacity + CANARY_LEFT_COUNT 
                          + CANARY_RIGHT_COUNT;
    StackUnit* tempPtr = (StackUnit*)calloc(capacityWithCanaries, sizeof(StackUnit));
    if (!tempPtr)
        return stk->status = MemoryAllocationError;

    stk->data = tempPtr;
    stk->size = 0;
    stk->capacity = initialCapacity;
    stk->capacityWithCanaries = capacityWithCanaries;
    stk->status = NaE;
    stk->isDestroyed = false;
    stk->isInitialized = true;
    for (size_t i = 0; i < CANARY_LEFT_COUNT; i++)
        stk->data[i] = CANARY_LEFT;
    for (size_t i = 0; i < CANARY_RIGHT_COUNT; i++)
        stk->data[stk->capacityWithCanaries - 1 - i] = CANARY_RIGHT;

    /*
    A way of doing all this via cpp's default struct values
    *stk = {.data = tempPtr,
            .capacity = initialCapacity,
            .isInitialized = true
            .status = NaE};
    */

    /*
    if (!(stackStatus = stackVerify(stkID)))
        return stackStatus;

    return stackStatus;
    */

    StackStatus status = stackVerify(stkID);
    //stackDumpInternal(stdout, stkID, true);
    return status == NaE ? stkID : status;
}

StackStatus stackPush(int stkID, StackUnit value){
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;

    StackStatus stackStatus = NaE;
    if ((stackStatus = stackVerify(stkID)))
        return stackStatus;

    size_t previousSize = stk->size;
    if (previousSize == stk->capacity) {
        StackUnit *tempPtr = (StackUnit*)realloc(stk->data,
                                                 (stk->capacityWithCanaries + stk->capacity)
                                                 * sizeof(StackUnit));
        if (!tempPtr)
            return stk->status = MemoryReallocationError;
        memset(tempPtr + CANARY_LEFT_COUNT + previousSize, 0, 
               (stk->capacity + CANARY_RIGHT_COUNT) * sizeof(StackUnit));
        stk->capacityWithCanaries += stk->capacity;
        stk->capacity *= 2;
        stk->data = tempPtr;
        for (size_t i = 0; i < CANARY_RIGHT_COUNT; i++)
            stk->data[stk->capacityWithCanaries - 1 - i] = CANARY_RIGHT;
    }

    stk->data[CANARY_LEFT_COUNT + stk->size++] = value;

    if (stk->size != previousSize + 1)
        return stk->status = IncrementationError;
    if (stk->data[CANARY_LEFT_COUNT + previousSize] != value)
        return stk->status = TransmutedValueError;

    return stackVerify(stkID);
}

StackUnit stackPop(int stkID, StackStatus *status){
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;

    StackStatus stackStatus = NaE;
    if ((stackStatus = stackVerify(stkID))) {
        if (status)
            *status = stk->status;
        return STACK_POISON;
    }

    size_t previousSize = stk->size;

    if (stk->size == 0) {
        stk->status = NothingToPopError;
        if (status)
            *status = stk->status;
        return STACK_POISON;
    }

    StackUnit value = stk->data[CANARY_LEFT_COUNT + (--stk->size)];
    stk->data[CANARY_LEFT_COUNT + stk->size] = 0;

    if (stk->size != previousSize - 1) {
        stk->status = DecrementationError;
        if (status)
            *status = stk->status;
        return value;
    }
    if (stk->data[CANARY_LEFT_COUNT + previousSize] != 0) {
        stk->status = FailedPopError;
        if (status)
            *status = stk->status;
        return value;
    }

    if (status)
        *status = stackVerify(stkID);
    return value;
}

StackStatus stackDestroy(int stkID){
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;

    if (stk->data) {
        for (size_t i = 0; i < stk->capacityWithCanaries; i++)
            stk->data[i] = STACK_POISON;
        free(stk->data);
        stk->data = NULL;
    }

    stk->size = 0;
    stk->capacity = 0;
    stk->status = InvalidStackID;
    stk->isInitialized = false;
    stk->isDestroyed = true;

    free(stk);
    STACK_MANAGER.data[stkID] = NULL;
    STACK_MANAGER.size--;
    if (STACK_MANAGER.size == 0) {
        free(STACK_MANAGER.data);
        STACK_MANAGER.data = NULL;
    }
    return NaE;
}

StackStatus stackVerify(int stkID){
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;
    if (!stk->data)
        return stk->status = NullDataPointerError;
    if (stk->capacity == 0)
        return stk->status = InvalidCapacityError;
    if (stk->size > stk->capacity)
        return stk->status = SizeOutOfBoundsError;
    return stk->status = stackCheckCanaries(stk);
}

StackStatus stackCheckCanaries(Stack* stk) {
    for (size_t i = 0; i < CANARY_LEFT_COUNT; i++)
        if (stk->data[i] != CANARY_LEFT) 
            return CorruptedCanaryError;
    for (size_t i = 0; i < CANARY_RIGHT_COUNT; i++)
        if (stk->data[stk->capacityWithCanaries - 1 - i] != CANARY_RIGHT) 
            return CorruptedCanaryError;
    return NaE;
}

int printFormattedStackUnitString(FILE* fileStream, Stack* stk, size_t index) {
    assert(fileStream);
    assert(stk);

    StackUnit value = stk->data[index];
    const char* prefix = (index >= CANARY_LEFT_COUNT 
                          && index < CANARY_LEFT_COUNT + stk->size
                          && value != STACK_POISON)
                          ?  "*" : " ";
    const char* suffix = (value != STACK_POISON) 
                          ? " " : "(POISON)";
    if (index < CANARY_LEFT_COUNT) {
        bool isCorrupted = (value != CANARY_LEFT);
        prefix = isCorrupted ? "!" : " ";
        suffix = isCorrupted ? "(CORRUPTED CANARY)" : "(CANARY)";
    } else if ((stk->capacityWithCanaries - 1 - index) < CANARY_RIGHT_COUNT) {
        bool isCorrupted = (value != CANARY_RIGHT);
        prefix = isCorrupted ? "!" : " ";
        suffix = isCorrupted ? "(CORRUPTED CANARY)" : "(CANARY)";
    }

    return fprintf(fileStream, "\t\t%s[%lu] = %d%s\n", prefix, index, value, suffix);
}

#undef stackDumpInternal

void stackDump(FILE *fileStream, int stkID,
               const char *fileName, int line) {
    assert(fileStream);

    return stackDumpInternal(fileStream, stkID, false, fileName, line);
}

void stackDumpInternal(FILE *fileStream, int stkID, bool isAdvanced,
               const char *fileName, int line) {
    assert(fileStream);

    Stack* stk = getStack(stkID);

    static long callCount = 0;

    static bool isRandomInitialized = false;
    if (!isRandomInitialized) {
        srand((unsigned int)time(NULL));
        isRandomInitialized = true;
    }

    StackStatus stackStatus = stackVerify(stkID);
    if (stk == NULL) {
        fprintf(fileStream,
                "-----------------------\n"
                "[Q] %s\n"
                "stackDump #%ld called from %s:%d\n"
                "Stack [NULL] {}\n"
                "-----------------------\n",
                QUOTES[(unsigned long)random()
                        % (sizeof(QUOTES) / sizeof(char *))],
                callCount++, fileName, line);
    } else {
        if (isAdvanced) {
            fprintf(fileStream,
                    "-----------------------\n"
                    "[Q] %s\n"
                   "stackDump #%ld called from %s:%d\n"
                    "Stack (stackID = %d) [%p]\n"
                    "{\n"
                    "\tsize         = %lu\n"
                    "\tcapacity     = %lu\n"
                    "\tcapacityWithCanaries = %lu\n"
                    "\t%serror = %d\n"
                    "\tisInitialized = %d\n"
                    "\tisDestroyed   = %d\n",
                    QUOTES[(unsigned long)random()
                            % (sizeof(QUOTES) / sizeof(char *))],
                    callCount++, fileName, line,
                    stkID, stk,
                    stk->size,
                    stk->capacity,
                    stk->capacityWithCanaries,
                    stk->status == NaE ? "" : "[!] ", stk->status,
                    stk->isInitialized,
                    stk->isDestroyed);
        } else {
            fprintf(fileStream,
                    "-----------------------\n"
                    "[Q] %s\n"
                   "stackDump #%ld called from %s:%d\n"
                    "Stack (stackID = %d)\n"
                    "{\n"
                    "\tsize         = %lu\n"
                    "\tcapacity     = %lu\n"
                    "\tcapacityWithCanaries = %lu\n"
                    "\t%serror = %d\n"
                    "\tisInitialized = %d\n"
                    "\tisDestroyed   = %d\n",
                    QUOTES[(unsigned long)random()
                            % (sizeof(QUOTES) / sizeof(char *))],
                    callCount++, fileName, line,
                    stkID,
                    stk->size,
                    stk->capacity,
                    stk->capacityWithCanaries,
                    stk->status == NaE ? "" : "[!] ", stk->status,
                    stk->isInitialized,
                    stk->isDestroyed);
        }

        if (stackStatus == NullDataPointerError || stk->data == NULL) {
            fprintf(fileStream,
                    "\tdata [NULL] {}\n"
                    "}\n"
                    "-----------------------\n");
        } else {
            if (isAdvanced) {
                fprintf(fileStream,
                        "\tdata [%p]\n"
                        "\t{\n",
                      stk->data);
            } else {
                fputs("\tdata\n"
                      "\t{\n", 
                      fileStream);
            }
            for (size_t i = 0; i < stk->capacityWithCanaries; i++) 
                printFormattedStackUnitString(fileStream, stk, i);
            fprintf(fileStream,
                    "\t}\n"
                    "}\n"
                    "-----------------------\n");
        }
    }
}

size_t stackGetSize(int stkID, StackStatus *status) {
    Stack* stk = getStack(stkID);
    if (!stk) {
        if (status) 
            *status = InvalidStackID;
        return 0;
    }

    return stk->size;
}

size_t stackGetCapacity(int stkID, StackStatus* status) {
    Stack* stk = getStack(stkID);
    if (!stk) {
        if (status) 
            *status = InvalidStackID;
        return 0;
    }

    return stk->capacity;
}

StackStatus stackGetError(int stkID) {
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;

    return stk->status;
}

Stack* getStack(int stkID) {
    return stkID < 0 || (unsigned int)stkID >= STACK_MANAGER.size 
           ? NULL 
           : STACK_MANAGER.data[stkID];
}