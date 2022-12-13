#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

char buf[1024];

typedef enum
{
    TYPE_LIST,
    TYPE_INT
} type_e;

struct element
{
    type_e t;
};
typedef struct element element;

struct integer
{
    element e;
    int value;
};
typedef struct integer integer;

struct list
{
    element e;
    element* Es;
    int nAlloc;
    int nUsed;
};
typedef struct list list;

list* list_create(void)
{
    list* ret = malloc(sizeof(list));
    if (ret)
    {
        ret->e.t = TYPE_LIST;
        ret->Es = NULL;
        ret->nAlloc = 0;
        ret->nUsed = 0;
    }
    return ret;
}

list_add_element(list* L, element* E)
{
    if (L->nUsed >= L->nAlloc)
    {
        L->nAlloc+=10;
        L->Es = realloc(L->Es, (sizeof(element*) * L->nAlloc));
        if (!L->Es)
        {
            fprintf(stderr, "Failure to allocate element points.\n");
            exit(-3);
        }
    }
}

integer* integer_create(int value)
{
    integer* ret = malloc(sizeof(integer));
    if (ret)
    {
        ret->e.t = TYPE_INT;
        ret->value = value;
    }
}

list* integer_promote_list(integer* I)
{
    list* l = list_create();
    list_add_element(l, (element*) I);
    return l;
}

void _populate_list(char** Line, list* L)
{
    char* line = *Line;

    while(*line != 0)
    {
        if (*line == '[')
        {
            list* l = list_create();
            line++;
            _populate_list(&line, l);
            list_add_element(l, (element*) l);
        }
        else if (*line == ',')
        {
            line++;
            continue;
        }
        else if (*line == ']')
        {
            line++;
            *Line = line;
            return;
        }
        else
        {
            if (isdigit(*line))
            {
                integer* I = integer_create(*line);
                list_add_element(L, (element*) I);
            }
            line++;
        }
    }
}

list* processLineToList(char* line)
{
    list* ret = list_create();
    if (ret)
    {
        line++;
        _populate_list(&line, ret);
    }
}

int main(int argc, char** argv, char** envp)
{
    FILE *fIn = 0;
    int status = 0;

    if (argc < 2)
    {
        printf("Not enough arguments");
        exit(-1);
    }

    fIn = fopen(argv[1], "r");
    if (fIn == NULL)
    {
        fprintf(stderr, "Unable to open file: %s\n%s\n", argv[1], strerror(errno));
        exit(-3);
    }
    else
    {
        ssize_t nChars = 0;

        // Process Packets
        unsigned int outOfOrderIndexRunning = 0;
        int currentIndex = 1;
        
        char* firstPacketLine = NULL;
        size_t firstPacketLength = 0;

        char* secondPacketLine = NULL;
        size_t secondPacketLength = 0;

        while(!feof(fIn))
        {
            
            nChars = getline(&firstPacketLine, &firstPacketLength, fIn);
            list* l1 = processLineToList(firstPacketLine);

            nChars = getline(&secondPacketLine, &secondPacketLength, fIn);
            list* l2 = processLineToList(secondPacketLine);

            currentIndex++;
        }

        free(line);
        fclose(fIn);
        fIn = NULL;

    }

    return 0;
}