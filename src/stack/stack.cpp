#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "stack.h"

static const StackUnit STACK_POISON = 0xAbD06;

static const StackUnit CANARY_LEFT  = (StackUnit)0xDEDDEDED;
static const StackUnit CANARY_RIGHT = (StackUnit)0xAAEDAEDA;

static const size_t CANARY_LEFT_COUNT  = 1;
static const size_t CANARY_RIGHT_COUNT = 1;

static StackStatus stackCheckCanaries(Stack* stk);

#ifdef _DEBUG

#undef stackDump

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

static int printFormattedStackUnitString(FILE* fileStream, Stack* stk, 
                                         size_t index);

static void stackDumpInternal(FILE* fileStream, Stack* stk, bool isAdvanced, 
                              const char* fileName, int line);
#define stackDumpInternal(fileStream, stk, isAdvanced) \
    stackDumpInternal(fileStream, stk, isAdvanced, __FILE__, __LINE__);

#else

#define stackDumpInternal(fileStream, stk, isAdvanced) ;

#endif

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

#ifdef _DEBUG

#undef stackDumpInternal

void stackDump(FILE *fileStream, Stack* stk,
               const char *fileName, int line) {
    assert(fileStream);

    return stackDumpInternal(fileStream, stk, false, fileName, line);
}

void stackDumpInternal(FILE *fileStream, Stack* stk, bool isAdvanced,
               const char *fileName, int line) {
    assert(fileStream);

    static long callCount = 0;

    static bool isRandomInitialized = false;
    if (!isRandomInitialized) {
        srand((unsigned int)time(NULL));
        isRandomInitialized = true;
    }

    StackStatus stackStatus = stackVerify(stk);
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
                    "Stack [%p]\n"
                    "{\n"
                    "\tsize         = %lu\n"
                    "\tcapacity     = %lu\n"
                    "\tcapacityWithCanaries = %lu\n"
                    "\t%serror = %d\n",
                    QUOTES[(unsigned long)random()
                            % (sizeof(QUOTES) / sizeof(char *))],
                    callCount++, fileName, line,
                    stk,
                    stk->size,
                    stk->capacity,
                    stk->capacityWithCanaries,
                    stk->status == OK ? "" : "[!] ", stk->status);
        } else {
            fprintf(fileStream,
                    "-----------------------\n"
                    "[Q] %s\n"
                   "stackDump #%ld called from %s:%d\n"
                    "Stack\n"
                    "{\n"
                    "\tsize         = %lu\n"
                    "\tcapacity     = %lu\n"
                    "\tcapacityWithCanaries = %lu\n"
                    "\t%serror = %d\n",
                    QUOTES[(unsigned long)random()
                            % (sizeof(QUOTES) / sizeof(char *))],
                    callCount++, fileName, line,
                    stk->size,
                    stk->capacity,
                    stk->capacityWithCanaries,
                    stk->status == OK ? "" : "[!] ", stk->status);
        }

        if (stackStatus == NullDataPointer || stk->data == NULL) {
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

#endif