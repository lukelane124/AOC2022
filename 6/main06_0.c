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

int insert(char* line)
{
    int map[128] = {0};
    int size = 0;
    int outIndex = 0;
    char* pEnd = line + strlen(line);
    char* pOut = line;
    while(line < pEnd)
    {
        
        char in = *line++;
        
        if (size++ < 4)
        {
            (map[in])++;
            fprintf(stderr, "In: %C\n", in);
        }
        else
        {
            char out = pOut[outIndex++];
            fprintf(stderr, "out: %C\n", out);
            fprintf(stderr, "In: %C\n", in);
            map[out]--;
            if (map[in] != 0)
            {
                (map[in])++;
            }
            else
            {
                if ((map[in] == 0) &&
                    (map[*(line-1)] == 1) &&
                    (map[*(line-2)] == 1) &&
                    (map[*(line-3)] == 1)
                )
                {
                    return size;
                }
                else
                {
                    (map[in])++;
                }
            }
        }
    }
    return 0;
}

int main(int argc, char** argv, char** envp)
{
    FILE *fIn = 0;
    int status = 0;
    char* filename = NULL;
    char dayNum = 0;
    static int map[128] = {0};

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
        int idx = 0;

        // Process Initial State.
        while(!feof(fIn))
        {
           nChars = getline(&line, &size, fIn);
           idx = insert(line);
           printf("index: %d\n", idx);
        }

        free(line);
        fclose(fIn);
        
        fIn = NULL;
    }

    return 0;
}