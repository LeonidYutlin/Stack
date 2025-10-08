#include "stack.h"
#undef stackDump

#ifndef STACK_POISON
#define STACK_POISON 0xAbD06
#endif

#ifndef CANARY_LEFT
#define CANARY_LEFT (StackUnit)0xDEDDEDED
#endif
#ifndef CANARY_RIGHT
#define CANARY_RIGHT (StackUnit)0xAAEDAEDA
#endif
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
    size_t trueCapacity = 0;
    StackError error = InvalidStackID;
    bool isDestroyed = false;
    bool isInitialized = false;
    //char state = '\0' 'd' 'i' 'e'
} Stack;

static const size_t INITIAL_STACK_MANAGER_CAPACITY = 5;
typedef struct StackManager {
    size_t size = 0;
    size_t capacity = 0;
    Stack** data = NULL;
} StackManager;
static StackManager STACK_MANAGER = {0};

static Stack* getStack(int stkID);
static StackError stackCheckCanaries(Stack* stk);
static int printFormattedStackUnitString(FILE* fileStream, 
                                         Stack* stk, size_t index);

static void stackDumpInternal(FILE* fileStream, int stkID, bool isAdvanced, 
                              const char* fileName, int line);
#define stackDumpInternal(fileStream, stkID, isAdvanced) \
    stackDumpInternal(fileStream, stkID, isAdvanced, __FILE__, __LINE__);

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

    size_t trueCapacity = initialCapacity + (size_t)CANARY_LEFT_COUNT 
                          + (size_t)CANARY_RIGHT_COUNT;
    StackUnit* tempPtr = (StackUnit*)calloc(trueCapacity, sizeof(StackUnit));
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
    if (!(stackError = stackVerify(stkID)))
        return stackError;

    return stackError;
    */

    StackError error = stackVerify(stkID);
    //stackDumpInternal(stdout, stkID, true);
    return error == NaE ? stkID : error;
}

StackError stackPush(int stkID, StackUnit value){
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;

    StackError stackError = NaE;
    if ((stackError = stackVerify(stkID)))
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

    return stackVerify(stkID);
}

StackUnit stackPop(int stkID, StackError *error){
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;

    StackError stackError = NaE;
    if ((stackError = stackVerify(stkID))) {
        if (error)
            *error = stk->error;
        return STACK_POISON;
    }

    size_t previousSize = stk->size;

    if (stk->size == 0) {
        stk->error = NothingToPopError;
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
        *error = stackVerify(stkID);
    return value;
}

StackError stackDestroy(int stkID){
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;

    if (*stk->data) {
        for (size_t i = 0; i < stk->trueCapacity; i++)
            stk->data[i] = STACK_POISON;
        free(stk->data);
        stk->data = NULL;
    }

    stk->size = 0;
    stk->capacity = 0;
    stk->error = InvalidStackID;
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

StackError stackVerify(int stkID){
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;
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

#undef stackDumpInternal

void stackDump(FILE *fileStream, int stkID,
               const char *fileName, int line) {
    assert(fileStream);

    return stackDumpInternal(fileStream, stkID, false, fileName, line);
}

void stackDumpInternal(FILE *fileStream, int stkID, bool isAdvanced,
               const char *fileName, int line) {
    assert(fileStream); //no assert?

    Stack* stk = getStack(stkID);

    static long callCount = 0;

    static bool isRandomInitialized = false;
    if (!isRandomInitialized) {
        srand((unsigned int)time(NULL));
        isRandomInitialized = true;
    }

    StackError stackError = stackVerify(stkID);
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
                    "\ttrueCapacity = %lu\n"
                    "\t%serror = %d\n"
                    "\tisInitialized = %d\n"
                    "\tisDestroyed   = %d\n",
                    QUOTES[(unsigned long)random()
                            % (sizeof(QUOTES) / sizeof(char *))],
                    callCount++, fileName, line,
                    stkID, stk,
                    stk->size,
                    stk->capacity,
                    stk->trueCapacity,
                    stk->error == NaE ? "" : "[!] ", stk->error,
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
                    "\ttrueCapacity = %lu\n"
                    "\t%serror = %d\n"
                    "\tisInitialized = %d\n"
                    "\tisDestroyed   = %d\n",
                    QUOTES[(unsigned long)random()
                            % (sizeof(QUOTES) / sizeof(char *))],
                    callCount++, fileName, line,
                    stkID,
                    stk->size,
                    stk->capacity,
                    stk->trueCapacity,
                    stk->error == NaE ? "" : "[!] ", stk->error,
                    stk->isInitialized,
                    stk->isDestroyed);
        }

        if (stackError == NullDataPointerError || stk->data == NULL) {
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
            for (size_t i = 0; i < stk->trueCapacity; i++) 
                printFormattedStackUnitString(fileStream, stk, i);
            fprintf(fileStream,
                    "\t}\n"
                    "}\n"
                    "-----------------------\n");
        }
    }
}

size_t stackGetSize(int stkID, StackError *error) {
    Stack* stk = getStack(stkID);
    if (!stk) {
        if (error) 
            *error = InvalidStackID;
        return 0;
    }

    return stk->size;
}

size_t stackGetCapacity(int stkID, StackError* error) {
    Stack* stk = getStack(stkID);
    if (!stk) {
        if (error) 
            *error = InvalidStackID;
        return 0;
    }

    return stk->capacity;
}

StackError stackGetError(int stkID) {
    Stack* stk = getStack(stkID);
    if (!stk)
        return InvalidStackID;

    return stk->error;
}

Stack* getStack(int stkID) {
    return stkID < 0 || (unsigned int)stkID >= STACK_MANAGER.size 
           ? NULL 
           : STACK_MANAGER.data[stkID];
}