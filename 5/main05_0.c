#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "km4_argument.h"

km4_arguments args;
char buf[1024];

void Usage(char* msg)
{
    fprintf(stderr, "Usage:\n\n\t%s <puzzle input file>\n", (char*) args.args_argv[0]);
    fprintf(stderr, "\nImproper invocation: %s\n\n", msg);
}

int main(int argc, char** argv, char** envp)
{
    FILE *fIn = 0;
    int status = 0;

    args.args_argc = argc;
    args.args_argv = argv;
    args.args_envp = envp;

    if (argc < 2)
    {
        Usage("Not enough arguments");
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
        char* line = NULL;
        size_t size = 0;
        ssize_t nChars = 0;
        unsigned int fullyContainedCount = 0;
        
        while(!feof(fIn))
        {
            nChars = getline(&line, &size, fIn);
        }

        fclose(fIn);
        fIn = NULL;

    }

    return 0;
}