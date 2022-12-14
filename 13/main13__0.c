#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
char buf[1024];

char* strUtil_dup(char* str, int i)
{
    char* ret = NULL;
    if (i < 0)
    {
        i = strlen(str);
    }

    ret = malloc(i + 1);
    if (ret)
    {
        memcpy(ret, str, i);
        ret[i] = 0;
    }
    return ret;
}

enum Element_e
{
    ETYPE_LIST,
    ETYPE_INT
};
typedef enum Element_e Element_e;



struct Element
{
    Element_e t;
    char* elementText;
};
typedef struct Element Element;

struct List
{
    Element E;
    Element** elements;
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
    // fprintf(stderr, "Finding closing brace for :%s\n", openingBrace);
    int depth = 0;
    char* pEntry = openingBrace;
    char* pEnd = openingBrace + strlen(openingBrace);
    pEnd--;
    openingBrace++;
    while(*openingBrace != 0)
    {
        if (*openingBrace == ']')
        {
            depth--;
            if (depth == -1)
                break;
        }
        else if (*openingBrace == '[')
        {
            depth++;
        }
        openingBrace++;
    }
    return ((openingBrace - pEntry) + 1);
}

void _list_populate(List* L, char* lText);

List* List_create(char* listText)
{
    List* l = malloc(sizeof(List));
    if (l)
    {
        int len = strlen(listText);
        Element* E = &l->E;
        E->elementText = malloc(len + 1);
        strcpy(E->elementText, listText);
        E->t = ETYPE_LIST;
        fprintf(stderr, "Creating List from: %s\n", listText);
        _list_populate(l, listText);
    }
    return l;
}

void _List_insert(List* L, Element* E)
{
    if (L->nItems >= L->nAlloc)
    {
        L->nAlloc += 10;
        L->elements = realloc(L->elements, (L->nAlloc * sizeof(Element*)));
    }
    L->elements[L->nItems++] = E; 
}

void _list_populate(List* L, char* lText)
{
    char* pCur = lText;
    char* pEnd = lText + strlen(lText);
    pCur++; // Don't handle the top level list.
    while(pCur < pEnd)
    {
        // fprintf(stderr, "[%d] pCur: %p\n", __LINE__, pCur);
        int depth = 0;
        if (*pCur == '[')
        {
            depth = _list_calculate_close_brace_index(pCur);
            // fprintf(stderr, "[%d] depth: %d\n", __LINE__, depth);
            // fprintf(stderr, "left to process: %s\n", pCur + depth+1);
            // Create and populate sub-list....
            // snprintf(buf, sizeof(buf), "%.*s", depth, pCur);
            // fprintf(stderr, "[%d] %s\n", __LINE__, buf);
            // Malloc a copy of the string??
            // fprintf(stderr, "pCur[depth]: %C\n", pCur[depth]);
            char* str = malloc(depth + 1);
            str[depth] = 0;
            memcpy(str, pCur, depth);
            pCur = pCur + depth;
            List* l = List_create(str);
            _List_insert(L, (Element*) l);
        }
        else if (*pCur == ']')
        {
            break;
        }
        else
        {
            if (*pCur == '\0' || *pCur == '\n')
            {
                break;
            }
            if (*pCur != ',')
            {
                // Should be value.
                // fprintf(stderr, "value: %C\n", *pCur);
                int val = *pCur - '0';
                // fprintf(stderr, "value: %d\n", val);
                Integer* i = malloc(sizeof(Integer));
                i->E.t = ETYPE_INT;
                i->E.elementText = strUtil_dup(pCur,1);
                i->i = val;
                _List_insert(L, (Element*) i);
                fprintf(stderr, "Value: %s\n", i->E.elementText);
            }
        }
        pCur++;
    }
}

List* _integer_promote_list(Integer* I)
{
    snprintf(buf, sizeof(buf), "[%1d]", I->i);
    fprintf(stderr, "Promoting %s to %s\n", I->E.elementText, buf);
    return List_create(buf);
}

int compare(Element* E1, Element* E2)
{
    fprintf(stderr, "%s <> %s\n", E1->elementText, E2->elementText);
    if (E1->t == ETYPE_INT && E2->t == ETYPE_INT)
    {
        Integer* i1 = (Integer*) E1;
        Integer* i2 = (Integer*) E2;
        int ret = (i1->i - i2->i);
        fprintf(stderr, "Comparing two integers: (%d,%d); returning: %d\n", i1->i, i2->i, ret);
        return ret;
    }
    else
    {
        if (E1->t == ETYPE_LIST && E1->t == ETYPE_LIST)
        {
            int ret = 0;
            List* l1 = (List*) E1;
            List* l2 = (List*) E2;
            int elementCount1 = l1->nItems;
            int elementCount2 = l2->nItems;
            int index = 0;
            while ( (elementCount1 > 0) && (elementCount2 > 0))
            {
                int result = compare(l1->elements[index], l2->elements[index]);
                if (result != 0)
                {
                    ret = result;
                    fprintf(stderr, "Sub compare resulted in <>, returning: %d\n", result);
                    break;
                }
                elementCount1--;
                elementCount2--;
                index++;
            }
            if (ret == 0 && (elementCount1 != elementCount2))
            {
                return elementCount1 - elementCount2;
            }
            else
            {
                return ret;
            }
        }
    }

    
    if ((E1->t == ETYPE_INT) && (E2->t == ETYPE_LIST) )
    {
    
        return compare((Element*) _integer_promote_list((Integer*) E1), (Element*) E2);
    }
    else if ((E2->t == ETYPE_INT) && (E1->t == ETYPE_LIST) )
    {
        return compare((Element*) _integer_promote_list((Integer*) E1),(Element*) E2);
        
    }
    else
    {
        fprintf(stderr, "Should never happen... LINE: %d\n", __LINE__);
        exit(-14);
    }

    fprintf(stderr, "Should never happen... LINE: %d\n", __LINE__);
    exit(-15);
}

void List_destroy(List* L)
{
    for (int i = 0; i < L->nItems; i++)
    {
        Element* E = (Element*) &L->elements[i];
        if (E->elementText)
        {
            free(E->elementText);
            E->elementText = NULL;
        }

        switch(E->t)
        {
            case ETYPE_LIST:
            {
                List* l = (List*) E;
                List_destroy(l);
            }
            case ETYPE_INT:
            default:
            {
                // Nothing else to free.
                // Element freed below.
                break;
            }
        }
        free(E);
    }
    free(L->elements);
    L->elements = NULL;
    free(L);
}

char* trim(char* str)
{
    char* ret = str;
    char* pBeg = str;
    char* pEnd = str + strlen(str);
    while(pBeg < pEnd)
    {
        if (isprint(*pBeg++))
        {
            pBeg--;
            break;
        }
    }

    while(pEnd>pBeg)
    {
        if (isprint(*pEnd))
        {
            pEnd++;
            *pEnd = 0;
            break;
        }
        pEnd--;
    }
    return pBeg;
}

int processPackets(char* p1, char* p2)
{
    int ret = 0;
    List* l1 = List_create(trim(p1));
    List* l2 = List_create(trim(p2));

    ret = compare((Element*) l1, (Element*) l2);
    fprintf(stderr, "compare resulted in :%d\n", ret);
    // List_destroy(l1);
    // List_destroy(l2);

    return ret;
}

int main(int argc, char** argv, char** envp)
{
    FILE *fIn = 0;
    int status = 0;

    if (argc < 2)
    {
        fprintf(stderr, "[%d] Not enough arguments\n", __LINE__);
        exit(-1);
    }

    fIn = fopen(argv[1], "r");
    if (fIn == NULL)
    {
        fprintf(stderr, "[%d] Unable to open file: %s\n%s\n", __LINE__, argv[1], strerror(errno));
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
            int processResults = processPackets(line1, line2);
            if (processResults <= 0)
            {
                fprintf(stderr, "Right order found at index: %d idx\n", idx);
                runningIndex += idx;
            }
            nChars = getline(&line2, &size2, fIn); // Clear empty newline between packets.
        }
        
        printf("running index: %u\n", runningIndex);

        free(line1);
        free(line2);

        fclose(fIn);
        fIn = NULL;

    }

    return 0;
}
