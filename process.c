#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    printf("process %d and its parent %d is called",getpid(),getppid());
    /*for(int i = 0; i < agrc; i++){

        printf(argv[i]);
        printf("\n");
    }*/
    remainingtime = atoi(argv[1]);
    int previousClk = -1;
    int clk ;
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    while (remainingtime > 0)
    {
        clk = getClk();

        if(previousClk != clk){
            remainingtime = remainingtime -1;
            previousClk = clk;
        }
    }
    
    printf("process %d and its parent %d is terminated",getpid(),getppid());
    kill(getppid(),SIGUSR1);

    destroyClk(false);
    
    return 0;
}
