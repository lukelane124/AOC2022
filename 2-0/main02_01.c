#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

char buf[1024];

typedef enum
{
    ROCK,
    PAPER,
    SCISSORS
} RPS_e;

#define selection_points(x) (x+1)

typedef enum
{
    I_WIN,
    I_DRAW,
    I_LOSS,
    I_COUNT
} resultPointsIndex_e;

int resultPoints[I_COUNT] = {6,3,0};

int resultIndex[3][3] = {
   //R,     P,      S
    {I_DRAW, I_LOSS, I_WIN}, //Rock
    {I_WIN, I_DRAW, I_LOSS}, //Paper
    {I_LOSS, I_WIN, I_DRAW}  //Scissors
};

RPS_e charToRPS(char c)
{
    switch(c)
    {
        case 'A':
        case 'X':
            return ROCK;
            break;

        case 'B':
        case 'Y':
            return PAPER;
            break;

        case 'C':
        case 'Z':
            return SCISSORS;
            break;
        default:
            fprintf("Encountered invalid character: %c\n", c);
            exit(-3);
    }
}

int charToResult(char c)
{
    switch (c)
    {
        case 'X':
            return I_LOSS;
            break;
        case 'Y':
            return I_DRAW;
            break;
        case 'Z':
            return I_WIN;
            break;
        default:
            fprintf(stderr, "Invalid character incountered\n");
            exit(-3);
    }
    exit(-4);
}

int selectionToResult(RPS_e opponentChoice, resultPointsIndex_e result)
{
    switch (result)
    {
        case I_LOSS:
        {
            if (opponentChoice == SCISSORS)
            {
                return PAPER;
            }
            else if (opponentChoice == ROCK)
            {
                return SCISSORS;
            }
            else
            {
                return ROCK;
            }
            break;
        }
        case I_DRAW:
        {
            return opponentChoice;
            break;
        }
        case I_WIN:
        {
            if (opponentChoice == SCISSORS)
            {
                return ROCK;
            }
            else if (opponentChoice == ROCK)
            {
                return PAPER;
            }
            else
            {
                return SCISSORS;
            }
            break;
        }
        default:
        {
            fprintf(stderr, "invalid choice.\n");
            exit(-5);
        }
    }
}

int main(int argc, char** argv, char** envp)
{
    FILE *fIn = fopen("strategy_guide.txt", "r");
    int status = 0;
    if (fIn == NULL)
    {
        perror("Unable to open file");
        exit(-3);
    }
    else
    {
        int runningPoints = 0;
        int selectionPoints;
        

        while(!feof(fIn))
        {
            fgets(buf, sizeof(buf), fIn);
            
            int opponentChoice = (int) charToRPS(buf[0]);
            int playerChoice = (int) charToRPS(buf[2]);
            int result = charToResult(buf[2]);
            
            runningPoints += resultPoints[result];

            for (int i = 0; i < 3; i++)
            {
                if (resultIndex[i][opponentChoice] == result)
                {
                    selectionPoints = i;
                    break;
                }
            }

            runningPoints += selection_points(selectionPoints);

            
        }

        printf("total points: %d\n", runningPoints);

        fclose(fIn);
        fIn = NULL;
    }

    return 0;
}