#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

char buf[1024];

int charToPrio(char c)
{
    if (c >= 'a')
    {
        return ((c-'a') + 1);
    }
    else
    {
        return ((c - 'A') + 27);
    }
}

int main(int argc, char** argv, char** envp)
{
    FILE *fIn = fopen("rucksack_contents.txt", "r");
    int status = 0;
    if (fIn == NULL)
    {
        perror("Unable to open file");
        exit(-3);
    }
    else
    {
        unsigned int runningPriority = 0;        
        char* line = NULL;
        size_t size = 0;
        
        while(!feof(fIn))
        {
            char map[128] = {0};
            int delim = '\n';
            char* pEnd = NULL;
            char* pCur = NULL;
            ssize_t byteCount = getline(&line, &size, fIn);
            byteCount--; //Remove newline byte.
            line[byteCount] = 0;
            byteCount /= 2;
            
            fprintf(stderr, "rucksack contains: %zd items.\n", byteCount*2);
            fprintf(stderr, "line: %s\n", line);
            
            
            pCur = line;
            pEnd = pCur + byteCount;
            
            fprintf(stderr, "first half: %.*s\n", (int)byteCount, pCur);
            
            while(pCur < pEnd)
            {
                int index = *pCur++;
                map[index] = 1;
            }
            
            pEnd = pEnd + byteCount;
            fprintf(stderr, "secon half: %s\n",pCur);
            
            while(pCur < pEnd)
            {
                int index = *pCur++;
                if (map[index] != 0)
                {
                    pCur--;
                    int prio = charToPrio(*pCur);
                    fprintf(stderr, "%c(%d) is already in the first compartment.\n", *pCur, prio);
                    runningPriority += prio;
                    break;
                }
            }            
        }

        fclose(fIn);
        fIn = NULL;
        free(line);
        printf("running priority: %u\n", runningPriority);
    }

    return 0;
}