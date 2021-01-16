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

char chosenAlgorithm;

int shm_msgSentNum_id; int* shm_msgSentNum_addr;
int sem_msgSentLock_id;
int sem_msgRcvdLock_id;
int msgq_process_id;

int sem_destroyResorces_id;

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
    return 0;
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
    printf("\nPlease Enter Scheduling Algorithm (0:rr, 1:hpf, 2:srtn): ");
    scanf("%c", &chosenAlgorithm);
    //printf("\nchosenAlgorithm = %d\n", chosenAlgorithm);
}

void initIpcs() {
    key_t shm_msgSentNum_key_id;
    shm_msgSentNum_addr = (int*) initShm(PROCESS_QUEUE_SHM_KEY_CHAR, sizeof(int), &shm_msgSentNum_key_id, &shm_msgSentNum_id);
    (*shm_msgSentNum_addr) = 0;


    key_t sem_msgSentLock_key_id;
    initSem(PROCESS_QUEUE_SHM_SENT_SEM_KEY_CHAR, 1, &sem_msgSentLock_key_id, &sem_msgSentLock_id);
    setValSem(sem_msgSentLock_id, 0, 0);
    printf("\nsem_msgSentLock_id = %d", sem_msgSentLock_id);
    

    key_t sem_msgRcvdLock_key_id;
    initSem(PROCESS_QUEUE_SHM_RCVD_SEM_KEY_CHAR, 1, &sem_msgRcvdLock_key_id, &sem_msgRcvdLock_id);
    setValSem(sem_msgRcvdLock_id, 0, 1);
    printf("\nsem_msgRcvdLock_id = %d", sem_msgRcvdLock_id);



    key_t msgq_process_key_id;
    initMsgQ(PROCESS_QUEUE_KEY_CHAR, &msgq_process_key_id, &msgq_process_id);

    key_t sem_destroyResorces_key_id;
    //initSem();
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
                execl("scheduler.out", "scheduler.out", &chosenAlgorithm, (char*) NULL);
            }
        } 
    }
}

void sendProcessEntry(ProcessEntry_t processEntry) {
    // send
    //printf("\nSending id = %d --- in time = %d", processEntry.entryId, currentTime);
    
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


    ProcessEntryNode_t* currentProcessNode = processEntriesHead;
    while (1)
    {
        //printf("\nprocess_generator.out -> %d\n", (*shmaddr));
        y = getClk();
        if (y - currentTime >= 1) {
            currentTime = y;
            //printf("\nprocess_generator.out -> currentTime = %d", currentTime);

            down(sem_msgRcvdLock_id); // Lock -> The only process that can work now is me (Scheduler waits till i finish sending).


            if (currentProcessNode == NULL) {
                (*shm_msgSentNum_addr) = -1; // To notify scheduler that no more processes can be sent.
                printf("\n\nNo processes remaining .....\n\n");
                
                up(sem_msgSentLock_id); // Lock -> Sending is now finished (Scheduler can start to recieve processes). I have to wait till it finishes recieving all processes.
                
                sleep(40);
                break;
            }
            else {

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
            }
            printf("\n______No of Processes sent from PG = %d at time %d", (*shm_msgSentNum_addr), currentTime);
            up(sem_msgSentLock_id); // Lock -> Sending is now finished (Scheduler can start to recieve processes). I have to wait till it finishes recieving all processes.
        }
    }
}
