#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

char buf[1024];

enum RPSMove_e
{
    ROCK,
    PAPER,
    SCISSORS,
    MOVE_COUNT
};
typedef enum RPSMove_e RPSMove_e;

char* RPSMoveText[MOVE_COUNT] = {
    "Rock",
    "Paper",
    "Scissors"
};

enum RPSRoundResult_e
{
    LOSE_POINTS = 0,
    DRAW_POINTS = 3,
    WIN_POINTS = 6
};
typedef enum RPSRoundResult_e RPSRoundResult_e;

struct RPSMatch
{
    RPSMove_e opponentChoice;
    RPSMove_e playerChoice;
    RPSRoundResult_e result;
};
typedef struct RPSMatch RPSMatch;

char* getNextMatch(char* fileContents, char** nextChar)
{
    char* newMatch = strstr(fileContents, "\n");
    if (newMatch && newMatch != fileContents)
    {
        *newMatch++ = 0;
        if (nextChar)
        {
            *nextChar = newMatch;
        }
        newMatch = fileContents;
    }
    return newMatch;
}

RPSMatch* createMatch(char* matchRow)
{
    RPSMatch* match = malloc(sizeof(RPSMatch));
    if (match)
    {
        char* tok = NULL;
        char* choices[2];
        choices[0] = strtok_r(matchRow, " ", &tok);
        choices[1] = strtok_r(NULL, " ", &tok);
        RPSMove_e moves[2];// = &match->opponentChoice;
        for (int i = 0; i < 2; i++)
        {
            switch(*choices[i])
            {
                case 'A':
                case 'X':
                {
                    // fprintf(stderr, "%s\n", RPSMoveText[ROCK]);
                    moves[i] = PAPER;
                    break;
                }
                case 'B':
                case 'Y':
                {
                    // fprintf(stderr, "%s\n", RPSMoveText[PAPER]);
                    moves[i] = ROCK;
                    break;
                }
                case 'C':
                case 'Z':
                {
                    // fprintf(stderr, "%s\n", RPSMoveText[SCISSORS]);
                    moves[i] = SCISSORS;
                    break;
                }
            }
        }
        match->opponentChoice = moves[1];
        match->playerChoice = moves[0];
    }
    
    // fprintf(stderr, "returning match\n");
    return match;
}

enum result_e
{
    LOSS,
    DRAW,
    WIN
};
typedef enum result_e result_e;

void printMatch(RPSMatch* match, FILE* fp)
{
    fprintf(fp, "%s VS %s\n", RPSMoveText[match->playerChoice], RPSMoveText[match->opponentChoice]);
}

/*
** Determine the outcome of the match and return the points the player gained for the match.
** Permute the *match to update the round points in the object when calculating the return values. 
*/
int calculateRoundResults(RPSMatch* match)
{
    int ret = 0;
    printMatch(match, stderr);

    static result_e resultMatrix[3][3] = {
        // ROCK VS
        {DRAW, LOSS, WIN},

        // PAPER VS
        {WIN, DRAW, LOSS},

        // SCISSORS VS
        {LOSS, WIN, DRAW}
    };

    result_e result = resultMatrix[(int)match->playerChoice][(int)match->opponentChoice];

    ret = match->playerChoice + 1;
    switch(result)
    {
        case WIN:
        {
            fprintf(stderr, "WIN\n");
            ret += 6;
            break;
        }
        case DRAW:
        {
            fprintf(stderr, "DRAW\n");
            ret += 3;
            break;
        }
        case LOSS:
        {
            fprintf(stderr, "LOSS\n");
            ret += 0;
            break;
        }
    }
    fprintf(stderr, "points: %d\n", ret);
    return ret;
}

int main(int argc, char** argv, char** envp)
{
    int fdIn = open("strategy_guide.txt", O_RDONLY);
    // fprintf(stderr, "Reading in file.\n");
    // fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
    char* fileContents = NULL;
    off_t fileSize = 0;
    int status = 0;
    off_t fileOffset = 0;
    // fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
    if (fdIn > 0)
    {
        // fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
        // fprintf(stderr, "Getting the length of the file.\n");
        fileSize = lseek(fdIn, 0, SEEK_END);
        fileContents = malloc(fileSize + 1);
        // fprintf(stderr, "file is %llo length in bytes.\n", fileSize);
        lseek(fdIn, 0, SEEK_SET);
        status = 0;

        fileOffset = 0;
        do {
            status = read(fdIn, buf, sizeof(buf));
            if (status > 0)
            {
                memcpy(&fileContents[fileOffset], buf, status);
                // fprintf(stderr, "read in:%d bytes\n", status);
                fileOffset += status;
            }
        }while(status > 0);
        fileContents[fileOffset] = 0;

        fprintf(stderr, "finished reading.\n");

        close(fdIn);
        fdIn = 0;
    }
    else
    {
        perror("Unable to open file");
        exit(-3);
    }
    // fprintf(stderr, "%s:%d\n", __FILE__, __LINE__);
    char* tok = NULL;


    fprintf(stderr, "Starting match processing loop.\n");
    char* match = strtok_r(fileContents, "\n", &tok);
    unsigned int cumulativeScore = 0;
    unsigned int processedLines = 0;
    while(match)
    {
        RPSMatch* round = createMatch(match);
        int resultPoints = calculateRoundResults(round);
        // fprintf(stderr, "Round result: %d\n", resultPoints);
        cumulativeScore += resultPoints;
        free(round);

        fileContents = tok;
        match = strtok_r(NULL, "\n", &tok);
        processedLines++;
    }
    fprintf(stderr, "processed lines: %u\n", processedLines);
    printf("My Score: %u\n", cumulativeScore);

    return 0;
}