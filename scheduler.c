#include "headers.h"
#include "IO/io.h"


//PCB pcbs[];
//PCB* newCommer;




int main(int argc, char * argv[])
{
    initClk();

    key_t shm_msgSentNum_key_id;
    int shm_msgSentNum_id;
    int* shm_msgSentNum_addr = (int*) initShm(PROCESS_QUEUE_SHM_KEY_CHAR, sizeof(int), &shm_msgSentNum_key_id, &shm_msgSentNum_id);


    key_t sem_msgSentLock_key_id;
    int sem_msgSentLock_id;
    initSem(PROCESS_QUEUE_SHM_SENT_SEM_KEY_CHAR, 1, &sem_msgSentLock_key_id, &sem_msgSentLock_id);

    key_t sem_msgRcvLock_key_id;
    int sem_msgRcvLock_id;
    initSem(PROCESS_QUEUE_SHM_RCVD_SEM_KEY_CHAR, 1, &sem_msgRcvLock_key_id, &sem_msgRcvLock_id);


    key_t key_id;
    int msgq_id;
    initMsgQ(PROCESS_QUEUE_KEY_CHAR, &key_id, &msgq_id);

    int currentTime = (*shmaddr);

    while (1)
    {
        //printf("\nprocess_generator.out -> %d\n", (*shmaddr));
        int y = getClk();
        if (y - currentTime >= 1) {
            currentTime = y;
            //printf("\nscheduler.out -> currentTime = %d", currentTime);
            
            down(sem_msgSentLock_id);

            for (int i = 0; i < (*shm_msgSentNum_addr); i++) {
                ProcessEntryMsgBuff_t message;
                int rec_val = msgrcv(msgq_id, &message, sizeof(message.p), 0, IPC_NOWAIT);
                if (rec_val == -1)
                    perror("Error in receive");
                else {
                    ProcessEntry_t val = message.p;
                    printf("\nMessage Recieved -> id = %d, arrival = %d, runtime = %d, priority = %d ---- in time = %d", val.entryId, val.arrival, val.runTime, val.priority, currentTime);
                }
            }

            up(sem_msgRcvLock_id);

        }



        if (currentTime >= 5) {
            break;
        }
    }

    //printf("\nI'm Scheduler.\n");
    
    //TODO implement the scheduler :)
    
    // if (chosenScheduler == rr) {
    //     rr_run(pcbs, newCommer);
    // } else if (hosenScheduler == hpf) {
    //     hpf_run(pcbs, newCommer);
    // }


    // Generate output files


    
    //upon termination release the clock resources.    
    destroyClk(/*true*/ false);
}
