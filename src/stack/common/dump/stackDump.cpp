#ifdef _DEBUG

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stackDump.h"
#include "../stackConst.h"

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

void stackDump(FILE *fileStream, Stack* stk, bool isAdvanced,
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