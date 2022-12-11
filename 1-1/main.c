#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

char buf[1024];

struct elfCalories
{
    int nItems;
    int nUsed;
    int* aItemCalories;
    unsigned int totalCalories;
    int index;
};
typedef struct elfCalories elfCalories;

int nElvesAlloc = 0;
elfCalories* aElves = NULL;
int nElvesUsed = 0;

elfCalories* createElfCalories(void)
{
    elfCalories* ret = NULL;
    if (nElvesAlloc <= nElvesUsed)
    {
        void* p = realloc(aElves, (nElvesAlloc + 10) * sizeof(elfCalories));
        if (p)
        {
            aElves = p;
            nElvesAlloc += 10;
            fprintf(stderr, "Alloc'd callories: %d\n", nElvesAlloc);
        }
    }

    if (nElvesAlloc > nElvesUsed)
    {
        fprintf(stderr, "Returning elf index: %d\n", nElvesUsed);
        aElves[nElvesUsed].index = nElvesUsed;
        ret = &aElves[nElvesUsed++];
    }
    return ret;
}

char* getNextElf(char* fileContents, char** nextChar)
{
    // fprintf(stderr, "%s(): fileContents: %s\n", __FUNCTION__, fileContents);
    // fprintf(stderr, "Grabbing next elf.\n");
    char* newElf = strstr(fileContents, "\n\n");
    if (newElf && newElf != fileContents)
    {
        *newElf++ = 0; *newElf++ = 0;
        if (nextChar)
        {
            *nextChar = newElf;
        }
        newElf = fileContents;
    }
    return newElf;
}

char** tokenizeInventoryLines(char* pbeg, int* LineCount)
{
    char** lines = NULL;
    char* tok;

    int linesAlloc = 0;

    int lineCount = 0;
    char* line = strtok_r(pbeg, "\n", &tok);
    while(line)
    {
        // fprintf(stderr, "line: %s\n", line);
        if (lineCount >= linesAlloc)
        {
            void* p = realloc(lines, (linesAlloc + 10) * sizeof(char*));
            if (p)
            {
                lines = p;
                linesAlloc += 10;
                // fprintf(stderr, "New max alloc lines: %d\nsize: %lu\n", linesAlloc, (linesAlloc * sizeof(char*)));
            }
        }

        if (linesAlloc > lineCount)
        {
            // fprintf(stderr, "Adding line: %s at index: %d\n", line, lineCount);
            lines[lineCount++] = line;
        }

        // fprintf(stderr, "Getting next line\n");
        line = strtok_r(NULL, "\n", &tok);
    }

    *LineCount = lineCount;
    // printf("lineCount: %d\n", lineCount);
    return lines;
}

int compare_elf_cal_total(const void* vp_e1, const void* vp_e2)
{
    const elfCalories *e1 = vp_e1, *e2 = vp_e2;
    return (e1->totalCalories - e2->totalCalories);
}

int main(int argc, char** argv, char** envp)
{
    int fdIn = open("calories.txt", O_RDONLY);
    char* fileContents = NULL;
    off_t fileSize = 0;
    int status = 0;
    off_t fileOffset = 0;

    if (fdIn > 0)
    {
        fileSize = lseek(fdIn, 0, SEEK_END);
        fileContents = malloc(fileSize + 1);
        lseek(fdIn, 0, SEEK_SET);
        status = 0;

        fileOffset = 0;
        // fprintf(stderr, "Filesize: %llo\n", fileSize);
        // fprintf(stderr, "Reading in file contents\n");
        do {
            status = read(fdIn, buf, sizeof(buf));
            // fprintf(stderr, "Read in %d bytes.\n", status);
            if (status > 0)
            {
                memcpy(&fileContents[fileOffset], buf, status);
                fileOffset += status;
            }
        }while(status > 0);
        fileContents[fileOffset] = 0;

        // fprintf(stderr, "finished reading.\n");

        close(fdIn);
        fdIn = 0;
    }

    while (1)
    {
        char* tok;
        char* elf = getNextElf(fileContents, &tok);
        // fprintf(stderr, "Elf: %256s\n", elf);
        // fprintf(stderr, "Next char: %10s\n", tok);

        fileContents = tok;
        if (elf)
        {
            int lineCount = 0;
            char** lines = tokenizeInventoryLines(elf, &lineCount);
            // fprintf(stderr, "lines: %p\n", lines);
            elfCalories* eCal = createElfCalories();
            if (lineCount && eCal)
            {
                eCal->nItems = lineCount;
                eCal->nUsed = 0;
                eCal->aItemCalories = malloc(lineCount * sizeof(int));
                eCal->totalCalories = 0;
                
                for (int i = lineCount; i > 0; i--)
                {
                    int cals = atoi(lines[lineCount-i]);
                    eCal->aItemCalories[eCal->nUsed++] = cals;
                    eCal->totalCalories += cals;
                }
            }
        }
        else
        {
            break;
        }
    }

    qsort(aElves, nElvesUsed, sizeof(elfCalories), compare_elf_cal_total);

    printf("\nelf Index: %d\nelf Calories: %u\n", aElves[nElvesUsed-1].index, aElves[nElvesUsed-1].totalCalories);

    unsigned int top3Cals = 0;
    for (int i = 0; i < 3; i++)
    {
        int index = nElvesUsed - i -1;
        top3Cals += aElves[index].totalCalories;
    }

    printf("Top 3 calorie count combined: %u\n", top3Cals);
}