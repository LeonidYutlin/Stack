#include "stack.h"
#undef stackDump

#ifndef STACK_POISON
#define STACK_POISON 0xAbD06
#endif

#ifndef CANARY_LEFT_COUNT
#define CANARY_LEFT_COUNT 1
#endif

#ifndef CANARY_RIGHT_COUNT
#define CANARY_RIGHT_COUNT 1
#endif

#ifndef CANARY_LEFT
#define CANARY_LEFT (StackUnit)0xDEDDEDED
#endif

#ifndef CANARY_RIGHT
#define CANARY_RIGHT (StackUnit)0xAAEDAEDA
#endif

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

static StackError stackCheckCanaries(Stack* stk);
static int printFormattedStackUnitString(FILE* fileStream, Stack* stk, size_t index);

StackError stackInit(Stack *stk, size_t initialCapacity){
    StackError stackError = NaE;
    if ((stackError = stackVerify(stk)) && stackError != UninitializedStackError)
        return stackError;

    if (stk->isInitialized)
        return stk->error = ReinitializationError;

    size_t trueCapacity = initialCapacity + (size_t)CANARY_LEFT_COUNT 
                          + (size_t)CANARY_RIGHT_COUNT;

    StackUnit *tempPtr = (StackUnit*)calloc(trueCapacity, sizeof(StackUnit));
    if (!tempPtr)
        return stk->error = MemoryAllocationError;

    stk->data = tempPtr;
    stk->size = 0;
    stk->capacity = initialCapacity;
    stk->trueCapacity = trueCapacity;
    stk->error = NaE;
    stk->isDestroyed = false;
    stk->isInitialized = true;
    for (size_t i = 0; i < (size_t)CANARY_LEFT_COUNT; i++)
        stk->data[i] = CANARY_LEFT;
    for (size_t i = 0; i < (size_t)CANARY_RIGHT_COUNT; i++)
        stk->data[stk->trueCapacity - 1 - i] = CANARY_RIGHT;

    /*
    A way of doing all this via cpp's default struct values
    *stk = {.data = tempPtr,
            .capacity = initialCapacity,
            .isInitialized = true
            .error = NaE};
    */

    /*
    if (!(stackError = stackVerify(stk)))
        return stackError;

    return stackError;
    */

    return stackVerify(stk);
}

StackError stackPush(Stack *stk, StackUnit value){
    StackError stackError = NaE;
    if ((stackError = stackVerify(stk)))
        return stackError;

    size_t previousSize = stk->size;
    if (previousSize == stk->capacity) {
        StackUnit *tempPtr = (StackUnit*)realloc(stk->data,
                                                 (stk->trueCapacity + stk->capacity)
                                                 * sizeof(StackUnit));
        if (!tempPtr)
            return stk->error = MemoryReallocationError;

        
        memset(tempPtr + (size_t)CANARY_LEFT_COUNT + previousSize, 0, 
               (stk->capacity + (size_t)CANARY_RIGHT_COUNT) * sizeof(StackUnit));
        stk->trueCapacity += stk->capacity;
        stk->capacity *= 2;
        stk->data = tempPtr;
        for (size_t i = 0; i < (size_t)CANARY_RIGHT_COUNT; i++)
            stk->data[stk->trueCapacity - 1 - i] = CANARY_RIGHT;
    }

    stk->data[(size_t)CANARY_LEFT_COUNT + stk->size++] = value;

    if (stk->size != previousSize + 1)
        return stk->error = IncrementationError;
    if (stk->data[(size_t)CANARY_LEFT_COUNT + previousSize] != value)
        return stk->error = TransmutedValueError;

    return stackVerify(stk);
}

StackUnit stackPop(Stack *stk, StackError *error){
    StackError stackError = NaE;
    if ((stackError = stackVerify(stk))) {
        if (error)
            *error = stk->error;
        return STACK_POISON;
    }

    size_t previousSize = stk->size;

    if (stk->size == 0) {
        stk->error = EmptyStackDataError;
        if (error)
            *error = stk->error;
        return STACK_POISON;
    }

    StackUnit value = stk->data[(size_t)CANARY_LEFT_COUNT + (--stk->size)];
    stk->data[(size_t)CANARY_LEFT_COUNT + stk->size] = 0;

    if (stk->size != previousSize - 1) {
        stk->error = DecrementationError;
        if (error)
            *error = stk->error;
        return value;
    }
    if (stk->data[(size_t)CANARY_LEFT_COUNT + previousSize] != 0) {
        stk->error = FailedPopError;
        if (error)
            *error = stk->error;
        return value;
    }

    if (error)
        *error = stackVerify(stk);
    return value;
}

StackError stackDestroy(Stack* stk){
    //simpler check.. just a simpler check
    // if (stackVerify(stk) == NullStackPointerError)
    if (!stk)
        return NullStackPointerError;

    if (*stk->data) {
        for (size_t i = 0; i < stk->trueCapacity; i++)
            stk->data[i] = STACK_POISON;
        free(stk->data);
        stk->data = NULL;
    }

    stk->size = 0;
    stk->capacity = 0;
    stk->error = DestroyedStackError;
    stk->isInitialized = false;
    stk->isDestroyed = true;

    //free(stk);

    return NaE;
}

StackError stackVerify(Stack *stk){
    if (!stk)
        return NullStackPointerError;
    if (stk->isDestroyed)
        return stk->error = DestroyedStackError;
    if (!stk->isInitialized)
        return stk->error = UninitializedStackError;
    if (!stk->data)
        return stk->error = NullDataPointerError;
    if (stk->capacity == 0)
        return stk->error = InvalidCapacityError;
    if (stk->size > stk->capacity)
        return stk->error = SizeOutOfBoundsError;
    return stk->error = stackCheckCanaries(stk);
}

StackError stackCheckCanaries(Stack* stk) {
    for (size_t i = 0; i < CANARY_LEFT_COUNT; i++)
        if (stk->data[i] != CANARY_LEFT) 
            return CorruptedCanaryError;
    for (size_t i = 0; i < CANARY_RIGHT_COUNT; i++)
        if (stk->data[stk->trueCapacity - 1 - i] != CANARY_RIGHT) 
            return CorruptedCanaryError;
    return NaE;
}

int printFormattedStackUnitString(FILE* fileStream, Stack* stk, size_t index) {
    assert(fileStream);
    assert(stk);

    StackUnit value = stk->data[index];
    const char* prefix = (index >= (size_t)CANARY_LEFT_COUNT 
                          && index < (size_t)CANARY_LEFT_COUNT + stk->size
                          && value != STACK_POISON)
                          ?  "*" : " ";
    const char* suffix = (value != STACK_POISON) 
                          ? " " : "(POISON)";
    if (index < CANARY_LEFT_COUNT) {
        bool isCorrupted = (value != CANARY_LEFT);
        prefix = isCorrupted ? "!" : " ";
        suffix = isCorrupted ? "(CORRUPTED CANARY)" : "(CANARY)";
    } else if ((stk->trueCapacity - 1 - index) < CANARY_RIGHT_COUNT) {
        bool isCorrupted = (value != CANARY_RIGHT);
        prefix = isCorrupted ? "!" : " ";
        suffix = isCorrupted ? "(CORRUPTED CANARY)" : "(CANARY)";
    }

    return fprintf(fileStream, "\t\t%s[%lu] = %d%s\n", prefix, index, value, suffix);
}

void stackDump(FILE *fileStream, Stack *stk,
               const char *fileName, int line) {
    assert(fileStream);

    static long callCount = 0;

    static bool isRandomInitialized = false;
    if (!isRandomInitialized) {
        srand((unsigned int)time(NULL));
        isRandomInitialized = true;
    }

    StackError stackError = stackVerify(stk);
    if (stackError == NullStackPointerError || stk == NULL) {
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
        fprintf(fileStream,
                "-----------------------\n"
                "[Q] %s\n"
                "stackDump #%ld called from %s:%d\n"
                "Stack [%p]\n"
                "{\n"
                "\tsize         = %lu\n"
                "\tcapacity     = %lu\n"
                "\ttrueCapacity = %lu\n"
                "\t%serror = %d\n"
                "\tisInitialized = %d\n"
                "\tisDestroyed   = %d\n",
                QUOTES[(unsigned long)random()
                        % (sizeof(QUOTES) / sizeof(char *))],
                callCount++, fileName, line,
                stk,
                stk->size,
                stk->capacity,
                stk->trueCapacity,
                stk->error == NaE ? "" : "[!] ", stk->error,
                stk->isInitialized,
                stk->isDestroyed);

        if (stackError == NullDataPointerError || stk->data == NULL) {
            fprintf(fileStream,
                    "\tdata [NULL] {}\n"
                    "}\n"
                    "-----------------------\n");
        } else {
            fprintf(fileStream,
                    "\tdata [%p]\n"
                    "\t{\n",
                    stk->data);
            for (size_t i = 0; i < stk->trueCapacity; i++) 
                printFormattedStackUnitString(fileStream, stk, i);
            fprintf(fileStream,
                    "\t}\n"
                    "}\n"
                    "-----------------------\n");
        }
    }
}