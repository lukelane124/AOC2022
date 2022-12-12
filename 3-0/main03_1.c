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
        int mapGroup[3][128] = {0};
        int elfIndex = 0;
        unsigned int runningGroupPriority = 0;
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
                mapGroup[elfIndex][index] = 1;
            }
            
            pEnd = pEnd + byteCount;
            fprintf(stderr, "secon half: %s\n",pCur);
            
            while(pCur < pEnd)
            {
                int index = *pCur++;
                mapGroup[elfIndex][index] = 1;
                if (map[index] != 0)
                {
                    pCur--;
                    int prio = charToPrio(*pCur);
                    fprintf(stderr, "%c(%d) is already in the first compartment.\n", *pCur, prio);
                    runningPriority += prio;
                    pCur++;
                    // break;
                }
            }
            elfIndex++;
            if (elfIndex == 3)
            {
                elfIndex = 0;
                for (int i = 'A'; i <= 'z'; i++)
                {
                    if (i > 'Z' && i < 'a')
                    {
                        i = 'a';
                    }

                    if (mapGroup[0][i] && mapGroup[1][i] && mapGroup[2][i])
                    {
                        runningGroupPriority += charToPrio(i);
                        fprintf(stderr, "found %c in all three rucksacks.\n", i);
                        break;
                    }
                }
                memset(mapGroup, 0, sizeof(mapGroup));
            }          
        }

        fclose(fIn);
        fIn = NULL;
        free(line);
        printf("running priority: %u\n", runningPriority);
        printf("running group priority: %u\n", runningGroupPriority);
    }

    return 0;
}