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

            resultPointsIndex_e index = resultIndex[playerChoice][opponentChoice];
            selectionPoints = selection_points(playerChoice);
            runningPoints += resultPoints[(int)index];
            runningPoints += selectionPoints;
        }

        printf("total points: %d\n", runningPoints);

        fclose(fIn);
        fIn = NULL;
    }

    return 0;
}