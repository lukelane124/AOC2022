#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
char buf[1024];

enum Element_e
{
    ETYPE_LIST,
    ETYPE_INT
};
typedef enum Element_e Element_e;



struct Element
{
    Element_e;
    char* elementText;
};
typedef struct Element Element;

struct List
{
    Element E;
    Element* elements;
    unsigned int nItems;
    unsigned int nAlloc;
};
typedef struct List List;

struct Integer
{
    Element E;
    int i;
};
typedef struct Integer Integer;

int _list_calculate_close_brace_index(char* openingBrace)
{
    int depth = 0;
    char* pEntry = openingBrace;
    openingBrace++;
    while(1)
    {
        if (*openingBrace++ == ']')
        {
            depth--;
            if (depth == 0)
            {
                return (openingBrace - pEntry);
            }
        }
        else if (*openingBrace++ == '[')
        {
            depth++;
        }
    }
}

List* List_create(char* listText)
{
    List* l = malloc(sizeof(List));
    if (l)
    {
        int len = strlen(listText);
        Element* E = &l->E;
        E->elementText = malloc(len + 1);
        _list_populate(l, listText);
    }
    return l;
}

void _list_populate(List* L, char* lText)
{
    char* pCur = lText;
    char* pEnd = lText + strlen(lText);
    pCur++; // Don't handle the top level list.
    while(pCur < pEnd)
    {
        int depth = 0;
        if (*pCur == '[')
        {
            depth = _list_calculate_close_brace_index(pCur);
            fprintf("depth: %d\n", depth);
            // Create and populate sub-list....
            snprintf(buf, sizeof(buf), "%.*s", depth, pCur);
            // Malloc a copy of the string??
            

            pCur = pCur + depth + 1;
        }
    }
}

int processPackets(char* p1, char* p2)
{
    List* l1 = List_create(p1);
    List* l2 = List_create(p2);
}

int main(int argc, char** argv, char** envp)
{
    FILE *fIn = 0;
    int status = 0;

    if (argc < 2)
    {
        fprintf(stderr, "Not enough arguments\n");
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
        char* line1 = NULL;
        size_t size1 = 0;
        char* line2 = NULL;
        size_t size2 = 0;
        ssize_t nChars = 0;
        unsigned int runningIndex = 0;
        unsigned int idx = 0;
        while(!feof(fIn))
        {
            nChars = getline(&line1, &size1, fIn);
            nChars = getline(&line2, &size2, fIn);
            idx++;
            // Process _packets_
            if (processPackets(line1, line2))
            {
                runningIndex += idx;
            }
        }

        free(line1);
        free(line2);

        fclose(fIn);
        fIn = NULL;

    }

    return 0;
}
