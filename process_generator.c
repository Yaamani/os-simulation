#include "headers.h"
#include "IO/io.h"
#include "DataStructures/linked_list.h"

void clearResources(int);

void askUserForChosenAlgorithm();
void initIpcs();
void forkClkAndScheduler();
void sendProcessEntry(ProcessEntry_t);
void mainLoop();


ProcessEntryNode_t* processEntriesHead = NULL;
ProcessEntryNode_t* processEntriesTail = NULL;

int chosenAlgorithm;

int shm_msgSentNum_id; int* shm_msgSentNum_addr;
int sem_msgSentLock_id;
int sem_msgRcvdLock_id;
int msgq_process_id;

int currentTime;


int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    
    // 1. Read the input files.
    readInputFile(&processEntriesHead, &processEntriesTail);

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    askUserForChosenAlgorithm();

    // 3. Initiate and create the scheduler and clock processes.
    initIpcs();
    forkClkAndScheduler();

    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    mainLoop();
        
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    msgctl(msgq_process_id, IPC_RMID, (struct msqid_ds *)0);
    shmdt(shm_msgSentNum_addr);
    //semctl();
}

void askUserForChosenAlgorithm() {
    // rr=0, hpf=1, srtn=2
    printf("\nPlease Enter Scheduling Algorithm (0:rr, 1:hpf, 2:2): ");
    scanf("%d", &chosenAlgorithm);
    //printf("\nchosenAlgorithm = %d\n", chosenAlgorithm);
}

void initIpcs() {
    key_t shm_msgSentNum_key_id;
    shm_msgSentNum_addr = (int*) initShm(PROCESS_QUEUE_SHM_KEY_CHAR, sizeof(int), &shm_msgSentNum_key_id, &shm_msgSentNum_id);
    (*shm_msgSentNum_addr) = 0;


    key_t sem_msgSentLock_key_id;
    initSem(PROCESS_QUEUE_SHM_SENT_SEM_KEY_CHAR, 1, &sem_msgSentLock_key_id, &sem_msgSentLock_id);
    setValSem(sem_msgSentLock_id, 0, 0);
    

    key_t sem_msgRcvdLock_key_id;
    initSem(PROCESS_QUEUE_SHM_RCVD_SEM_KEY_CHAR, 1, &sem_msgRcvdLock_key_id, &sem_msgRcvdLock_id);
    setValSem(sem_msgRcvdLock_id, 0, 1);


    key_t msgq_process_key_id;
    initMsgQ(PROCESS_QUEUE_KEY_CHAR, &msgq_process_key_id, &msgq_process_id);
}

void forkClkAndScheduler() {
    int pid;
    for (int i = 0; i < 2; i++) {
        pid = fork();
        if (pid == 0) { // Child
            if (i == 0) {
                printf("\nI'm now child.\n");
                execl("clk.out", "clk.out", (char*) NULL);
            } else {
                execl("scheduler.out", "scheduler.out", (char*) NULL);
            }
        } 
    }
}

void sendProcessEntry(ProcessEntry_t processEntry) {
    // send
    printf("\nSending id = %d --- in time = %d", processEntry.entryId, currentTime);
    
    ProcessEntryMsgBuff_t msg;
    msg.type = 5;
    msg.p = processEntry;

    int send_val = msgsnd(msgq_process_id, &msg, sizeof(msg.p), !IPC_NOWAIT);
    if (send_val == -1)
        perror("Errror in send");
}

void mainLoop() {
    int y = 0;
    // To get time use this
    currentTime = getClk();
    
    //bool mess_sent = 0;
    
    ProcessEntryNode_t* currentProcessNode = processEntriesHead;
    while (1)
    {
        //printf("\nprocess_generator.out -> %d\n", (*shmaddr));
        y = getClk();
        if (y - currentTime >= 1) {
            currentTime = y;
            printf("\nprocess_generator.out -> currentTime = %d", currentTime);

            //if(mess_sent) 
                down(sem_msgRcvdLock_id);

            (*shm_msgSentNum_addr) = 0;
            while (currentProcessNode)
            {
                ProcessEntry_t val = currentProcessNode->val;
                if (val.arrival == currentTime) {
                    (*shm_msgSentNum_addr)++;
                    
                    sendProcessEntry(val);
                    
                    currentProcessNode = currentProcessNode->next;

                    // Delete node
                    removeFirst_ProcessEntry(&processEntriesHead);

                } else {
                    break;
                }
            }

            up(sem_msgSentLock_id);
            //mess_sent = 1;
        }

        if (currentTime >= 5)
            break;
    }
}
