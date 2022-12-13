#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "AOC_utils.h"

char buf[1024];

typedef struct stack_item
{
    int value;
    struct stack_item* previous;
} stack_item;

stack_item* stack_item_create(int value)
{
    stack_item* ret = malloc(sizeof(stack_item));
    if (!ret)
    {
        perror("malloc for stack_item failed...");
        exit(-3);
    }
    else
    {
        ret->previous = NULL;
        ret->value = value;
    }
    return ret;
}

struct Stack
{
    stack_item* top;
    unsigned int numItems;
};
typedef struct Stack Stack;

void stack_push(Stack* stack, stack_item* item)
{
    if (stack->numItems == 0)
    {
        stack->top = item;
    }
    else
    {
        item->previous = stack->top;
        stack->top = item;
    }
    stack->numItems++;
}

stack_item* stack_pop(Stack* stack)
{
    stack_item* ret = stack->top;
    if (ret->previous == NULL)
    {
        stack->top = NULL;
        assert(stack->numItems == 1);
    }
    else
    {
        stack->top = ret->previous;
    }
    ret->previous = NULL;
    stack->numItems--;
    return ret;
}

#define stack_peek(s) (s->top->value)
#define stack_is_empty(s) (((s)->top) == NULL)

int determineNumberOfStacks(char* line)
{
    int ret = 0;
    int subIndex = 0;
    while(*line != '\n' && *line != '\0')
    {
        subIndex++;
        line++;
    }

    while(subIndex > 0)
    {
        subIndex -= 4;
        ret++;
        // subIndex--;
    }
    return ret;
}

Stack* stacks = NULL;
int nStacks;

void processStackLine(char* line)
{
    char* pEnd = line + strlen(line);
    int stackIndex = 0;
    line++;

    while(*line != '\n' && *line != '\0' && line < pEnd)
    {
        if (*line != ' ')
        {
            stack_item* nCrate = stack_item_create(*line);
            stack_push(&stacks[stackIndex], nCrate);
        }
        line += 4;
        stackIndex++;
    }

}

void printStack(FILE* F, Stack* stack)
{
    stack_item* cur = stack->top;
    while(cur != NULL)
    {
        fprintf(F, "[%c]\n", cur->value);
        cur = cur->previous;
    }
    fprintf(F, "\n");
}

void reverseStack(Stack* stackIn)
{
    Stack* tempStack = &stacks[nStacks+1];
    Stack* sourceStack = stackIn;
    
    while(!stack_is_empty(sourceStack))
    {
        stack_push(tempStack, stack_pop(sourceStack));
    }

    *sourceStack = *tempStack;
    tempStack->numItems = 0;
    tempStack->top = NULL;
}

void processMove(char* line)
{
    int num, src, dest;
    char *tok = NULL;
    // fprintf(stderr, "move line:\n%s", line);
    // Move.
    assert(3 == sscanf(line, "move %d from %d to %d\n", &num, &src, &dest));
    // fprintf(stderr, "move %d from %d to %d\n", num, src, dest);

    src--;
    dest--;

    // Reflect movement on stacks.
    // printStack(stderr, &stacks[src]);
    // printStack(stderr, &stacks[dest]);
    while(num)
    {
        if (!stack_is_empty(&stacks[src]))
        {
            stack_push(&stacks[dest], stack_pop(&stacks[src]));
        }
        num--;
    }
    // printStack(stderr, &stacks[dest]);
}

int main(int argc, char** argv, char** envp)
{
    FILE *fIn = 0;
    int status = 0;
    char* filename = NULL;
    char dayNum = 0;

    if (argc < 2)
    {
        AOC_Usage(argv[0], "Not enough arguments");
    }
    
    fIn = fopen(argv[1], "r");

    if (fIn == NULL)
    {
        fprintf(stderr, "Unable to open file: %s\n%s\n", argv[1], strerror(errno));
        exit(-3);
    }
    else
    {
        char* line = NULL;
        size_t size = 0;
        ssize_t nChars = 0;

        nChars = getline(&line, &size, fIn);
        nStacks = determineNumberOfStacks(line);
        fprintf(stderr, "Found: %d Stacks\n", nStacks);

        stacks = malloc(sizeof(Stack) * (nStacks + 1)); // Extra stack for temp storage.
        for (int i = 0; i < nStacks; i++)
        {
            stacks[i].numItems = 0;
            stacks[i].top = NULL;
        }

        processStackLine(line);
        
        // Process Initial State.
        while(!feof(fIn))
        {
            nChars = getline(&line, &size, fIn);
            if (line[0] == '\n' || line[0] != '[')
            {
                break;
            }
            else
            {
                processStackLine(line);
            }
        }

        for (nChars = 0; nChars < nStacks; nChars++)
        {
            reverseStack(&stacks[nChars]);
            // printStack(stderr, &stacks[nChars]);
            // fprintf(stderr, "\n");
        }

        nChars = getline(&line, &size, fIn);

        //Process Movements.
        while(!feof(fIn))
        {
            nChars = getline(&line, &size, fIn);

            processMove(line);
        }

        free(line);

        nChars = 0;
        for (int i = 0; i < nStacks; i++)
        {
            buf[nChars++] = stack_peek((&stacks[i]));
        }
        buf[nChars] = 0;
        fclose(fIn);
        printf("finalOrder: %s\n", buf);
        fIn = NULL;

    }

    return 0;
}