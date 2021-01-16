#include "headers.h"


int sem_processFinishesFirstLock_id;
int sem_processStartsAfterSchdulerPermission_id;

int shm_processEnded_id;
int* shm_processEnded_addr;


/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{


    key_t sem_processFinishesFirstLock_key_id;
    initSem(PROCESS_FINISHES_FIRST_SEM_KEY_CHAR, 1, &sem_processFinishesFirstLock_key_id, &sem_processFinishesFirstLock_id);
    
    key_t sem_processStartsAfterSchdulerPermission_key_id;
    initSem(PROCESS_SHEDULER_PERMISSION_FIRST_SEM_KEY_CHAR, 1, &sem_processStartsAfterSchdulerPermission_key_id, &sem_processStartsAfterSchdulerPermission_id);


    key_t shm_processEnded_key_id;
    shm_processEnded_addr = (int*) initShm(PROCESS_ENDED_SHM_KEY_CHAR, sizeof(int), &shm_processEnded_key_id, &shm_processEnded_id);




    initClk();
    printf("\n WWWWWWWWWWW process %d and its parent %d is called \n",getpid(),getppid());
    /*
    for(int i = 0; i < agrc; i++){

        printf("\n %s ",argv[i]);
        printf("\n");
    }
    */
    remainingtime = atoi(argv[1]);
    //printf("\n clk : %d , remaining time : %d \n", getClk(), remainingtime);
    int previousClk = getClk();
    //int previousClk = 0;
    int clk ;
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    while (remainingtime > 0)
    {
        clk = getClk();

        if(previousClk != clk){
            
            //printf("\n clk : %d , remaining time : %d \n", clk , remainingtime-1);
            remainingtime = remainingtime -1;
            
            previousClk = clk;
            
            if (remainingtime == 0) {
                *shm_processEnded_addr = 1;
                //kill(getppid(),SIGUSR1);
            }

            // UP
            printf("\n ***** UP SEMAPHORE **** \n");
            up(sem_processFinishesFirstLock_id);
            
        }
    }
    
    printf("\n process %d and its parent %d is terminated \n",getpid(),getppid());
   
    destroyClk(false);
    
    return 0;
}
