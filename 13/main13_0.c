#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
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

void _list_print(FILE* F, list* List)
{
    int isFirst = 1;
    fprintf(F, "[");
    element* E;
    list* L;
    integer* I;
    for (int i = 0; i < List->nUsed; i++)
    {
        E = &List->Es[i];
        if (!isFirst)
        {
            fprintf(F,", ");
        }
        switch(E->t)
        {
            case TYPE_INT:
            {
                I = (integer*) E;
                fprintf(F, "%d", I->value);
                break;
            }
            case TYPE_LIST:
            {
                L = (list*) E;
                _list_print(F,L);
            }
        }
       
    }
}

void list_print(list* L)
{
    _list_print(stderr, L);
}

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

void list_add_element(list* L, element* E)
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
    return ret;
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

element* _processChars(char** Line)
{
    element* ret = NULL;
    char* line = *Line;
    if (isdigit(*line))
    {
        integer* I = integer_create(*line);
        line++;
        *Line = line;
        ret = (element*) I;
    }

    return ret;
}

list* processLineToList(char** Line)
{
    char*  line = *Line;
    list* ret = list_create();
    if (ret)
    {
        while(*line != '\0')
        {
            switch (*line++)
            {
                case '[':
                {
                    list_add_element(ret, (element*) processLineToList(&line));
                    break;
                }
                case ']':
                {
                    *Line = line;
                    return ret;
                }
                case ',':
                {
                    continue;
                    break;
                }
                default:
                {
                    list_add_element(ret, _processChars(&line));
                }
            }
        }
        
        
        list_print(ret);
    }
    return ret;
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
        
        char* l1 = NULL;
        size_t firstPacketLength = 0;

        char* l2 = NULL;
        size_t secondPacketLength = 0;

        while(!feof(fIn))
        {
            
            nChars = getline(&l1, &firstPacketLength, fIn);
            list* L1 = processLineToList(&l1);

            nChars = getline(&l2, &secondPacketLength, fIn);
            list* L2 = processLineToList(&l2);

            

            currentIndex++;
        }

        fclose(fIn);
        fIn = NULL;

    }

    return 0;
}