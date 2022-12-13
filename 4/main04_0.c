#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

char buf[1024];

int ranges[2][2];

int contained(void)
{
    int ret = 0;

    if (ranges[0][0] >= ranges[1][0] && ranges[0][1] <= ranges[1][1])
    {
        // first ranges possibly contains the second ranges.
        ret = 1;
    }
    else if (ranges[0][0] <= ranges[1][0] && ranges[1][1] <= ranges[0][1])
    {
        // Second ranges possibley contains the first ranges.
        ret = 1;
    }
    return ret;
}

int main(int argc, char** argv, char** envp)
{
    FILE *fIn = fopen("assignment_pairs.txt", "r");
    int status = 0;
    if (fIn == NULL)
    {
        perror("Unable to open file");
        exit(-3);
    }
    else
    {
        char* line = NULL;
        size_t size = 0;
        ssize_t nChars = 0;
        unsigned int fullyContainedCount = 0;
        
        while(!feof(fIn))
        {
            nChars = getline(&line, &size, fIn);
            char* pairTok = NULL;
            char* p1 = strtok_r(line, ",", &pairTok);
            char* p2 = strtok_r(NULL, ",", &pairTok);

            

            char* rangesTok = NULL;
            char* p1ranges = strtok_r(p1, "-", &rangesTok);
            ranges[0][0] = atoi(p1ranges);
            p1ranges = strtok_r(NULL, "-", &rangesTok);
            ranges[0][1] = atoi(p1ranges);

            char* p2ranges = strtok_r(p2, "-", &rangesTok);
            ranges[1][0] = atoi(p2ranges);
            p2ranges = strtok_r(NULL, "-", &rangesTok);
            ranges[1][1] = atoi(p2ranges);

            if (contained())
            {
                fullyContainedCount++;
            }
        }

        fclose(fIn);
        fIn = NULL;
        printf("fullContainedCound: %u\n", fullyContainedCount);        
    }

    return 0;
}