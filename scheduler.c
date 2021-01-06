#include "headers.h"
#include "IO/io.h"
#include "DataStructures/linked_list.h"

#define rr 0
#define hpf 1
#define srtf 2 

#define watch(x) printf(#x "%d \n",x)
#define watchF(x) printf(#x "%f \n",x)
#define watchS(x) printf( #x "%s \n",x)

//update
#define max 1000000
PCB_t pcbs[5];

//update
const int timeSahring = 5;

PCBNode_t* pcbHead = NULL;
EventNode_t* eventsHead = NULL;

int clk,chosenScheduler;
int previousClk = 0;

bool endProgram = false;
bool process_ended = true;

//update
PCBNode_t* current_running = NULL; // for roundRoben

void readNewProcess(); // yamani

// int getChosenScheduler(){} //yamani

void testing(int i,int proiority,int runtime);

void runProcess(int id);

int hpf_run();

int rr_run();

int shortestTimeFinish_run();

//update
void copyData(PCBNode_t* current);

void handler(int signum);

void printEvents();

void printEvent(Event_t event);

void IPCs();

int shm_msgSentNum_id;
int* shm_msgSentNum_addr;
int sem_msgSentLock_id;
int sem_msgRcvdLock_id;
int msgq_id;


int currentTime;


int main(int argc, char * argv[])
{
    
    chosenScheduler  = argv[1][0]-'0';
    printf("\nshceduler.c -> chosenAlgorithm = %d", chosenScheduler);

    signal(SIGUSR1,handler);

    IPCs();

    initClk();

    //currentTime = getClk();

    int id;

    //update
    current_running = (PCBNode_t*) malloc(sizeof(PCBNode_t));

    while(get_size(pcbHead) != 0 || !endProgram){

        clk = getClk();
        
        if(previousClk != clk){

            printf("\n XXXXXXXsize of PCB = %d, end program value = %d", get_size(pcbHead), endProgram);
            previousClk = clk;
            printf("\n******* Going to Read New Procs at time %d", getClk());
            readNewProcess();
            
            switch (chosenScheduler)
            {
                case rr:
                    id = rr_run();
                    break;
                case hpf:
                    id = hpf_run();
                    printf("\n process id %d that will run \n",id);
                    break;
                case srtf:
                    id = shortestTimeFinish_run();
                    break;    
            }
            //update
            if(id != -1){
                runProcess(id);
            }
            
        }
    }

    // Generate output files


    
    //upon termination release the clock resources. 
    
    //printEvents();
    printf("\n Scheduler is Terminated \n");

    free(current_running); //be sure delete by & or not
    current_running = NULL;
    
    destroyClk(/*true*/ false);
}

int hpf_run(){

    if(pcbHead == NULL || !process_ended){

        return -1;
    }
    PCBNode_t* current = pcbHead; 
    
    int highest_p = -1;
    int id;

    while(current){

        if(current->val.priority > highest_p){

            highest_p = current->val.priority;
            id = current->val.entryId;
        }
        current = current->next;
    }
    //process_ended = false;
    return id;
}

//update

int rr_run(){


    if(pcbHead == NULL){

        return -1;
    }
    
    //PCBNode_t* current = pcbHead;
    if(!process_ended){
        
        
        if( clk - current_running->val.lastStartedTime > timeSahring){

            if(current_running->next != NULL){
                //current_running = current_running->next; 
                copyData(current_running->next);
                return current_running->next->val.entryId;
            }
            copyData(pcbHead);
            //current_running = pcbHead; 
            return pcbHead->val.entryId;

        }
        return -1;

    }
    else{

        process_ended = false;
        if(current_running->next != NULL){
            //current_running = current_running->next;
            copyData(current_running->next);
            return current_running->next->val.entryId;
        }
        //current_running = pcbHead;
        copyData(pcbHead);
        return pcbHead->val.entryId;
        
    }
   
}

//update

int shortestTimeFinish_run(){

     if(pcbHead == NULL){
        return -1;
    }
    int shortest_Remainig_Time = max;
    int id;
    PCBNode_t* current = pcbHead;

    // we can use current_runnig instead of each time call while loop
    // then compare current_runnig with new process enter (clk == arrivalTime)
    // we can make linked list return the last element of linked list which is called in our case new process
    int remainingTime = 0;
    while (current)
    {
        remainingTime = current->val.remainingTime ;
        if(current->val.isRunning){
            remainingTime =  current->val.remainingTime - (clk -  current->val.lastStartedTime );
        }
        // remainingTime != 0 if the process is ended
        if(remainingTime  < shortest_Remainig_Time && remainingTime != 0){

            shortest_Remainig_Time = current->val.remainingTime;
            id = current->val.entryId;
        }
    }
    if(current->val.isRunning){
        return -1;
    }
    return id; 
}

void runProcess(int id){

    if(id == -1){
        return;
    }

    PCBNode_t* current = pcbHead; 
    Event_t event;

    while(current){

        if(current->val.isRunning){
            
            //add new event
            current->val.remainingTime =  current->val.remainingTime - (clk - current->val.lastStartedTime);

            event.entryId = current->val.entryId;
            event.remaining =  current->val.remainingTime;
            event.time = clk;
            event.total = current->val.runTime;
            event.arrival = current->val.arrivalTime;
            event.wait = (clk - current->val.arrivalTime) - (current->val.runTime-current->val.remainingTime);
           
            if(!process_ended) {

                event.turnaroundTime = -1;
                event.weightedTurnaroundTime = -1;
                event.state = STOPPED;

                kill(current->val.pid,SIGSTOP);
                current->val.isRunning = false;
            }
            else {
                event.state = FINISHED;
                event.turnaroundTime = clk - current->val.arrivalTime;
                event.weightedTurnaroundTime = (float)event.turnaroundTime / current->val.runTime;
                delete_PCB(&pcbHead,id);
            }
            push_Event(&eventsHead,event);
            printEvent(event);
            break;    
        }
        current = current->next;

    }
    current = get_pcb(pcbHead,id);

    event.entryId = current->val.entryId;
    event.remaining =  current->val.remainingTime;
    event.time = clk;
    event.total = current->val.runTime;
    event.arrival = current->val.arrivalTime;
    event.wait = (clk - current->val.arrivalTime) - (current->val.runTime-current->val.remainingTime);
    event.turnaroundTime = -1;
    event.weightedTurnaroundTime = -1;

    // if current is paused resume and set lastStartedRunning 
    if(current->val.pid != -1){

        kill(current->val.pid,SIGCONT);
        current->val.lastStartedTime = clk;

        event.state = RESUMED;
    }
    else{
        event.state = STARTED;
        //update
        current->val.startingTime = clk;
        current->val.lastStartedTime = clk;
        int pid = fork();
        if(pid == -1){
            perror("error in forking");
        }
        else if(pid == 0){

            // need to be tested
            printf("\n CREATE PROCESS id = %d \n",id);
            execl("process.out","process.out", &(current->val.remainingTime), (char*) NULL);

        }

        current->val.pid = pid;

    }

    push_Event(&eventsHead,event);
    printEvent(event);
    current->val.isRunning = true; 
    process_ended= false; 

}

//update
void copyData(PCBNode_t* current){

    current_running->val.isRunning = current->val.isRunning;
    current_running->next = current->next;
    current_running->val.entryId = current->val.entryId;
}

void handler(int signum){

    process_ended = true;
    printf("\n handler is Called \n");
}

void testing(int i,int proiority,int runtime){

    PCB_t pcb[i];
    pcb[i].entryId = i;
    pcb[i].isRunning = false;
    pcb[i].lastStartedTime = -1;
    pcb[i].pid = -1;
    pcb[i].priority = proiority;
    pcb[i].remainingTime = runtime;
    pcb[i].runTime = runtime;
    pcb[i].startingTime = -1;
    pcb[i].arrivalTime = i;

    push_PCB(&pcbHead,pcb[i]);
}

void printEvents(){

    EventNode_t* current = eventsHead;

    if(current == NULL){

        printf("Error in Print Events\n");
        printf("There is no events\n\n");

        return;
    }
    int i = 1;
    while(current){

        printf("Event%d\n",i);
        watch(current->val.entryId);
        watch(current->val.remaining);
        switch (current->val.state)
        {
        case STARTED:
            watchS("STARTED");
            break;
        case STOPPED:
            watchS("STOPPED");
            break;
        case FINISHED:
            watchS("FINISHED");
            break;
        case RESUMED:
            watchS("RESUMED");
            break;    
        }
        watch(current->val.time);
        watch(current->val.total);
        watch(current->val.wait);
        watch(current->val.arrival);
        if(current->val.turnaroundTime != -1){
            watch(current->val.turnaroundTime);
        }
        if(current->val.weightedTurnaroundTime != -1){

            watchF(current->val.weightedTurnaroundTime);

        }
        printf("-------------------------\n");
        i = i + 1;
        current = current->next;
    }

}

void IPCs(){

    key_t shm_msgSentNum_key_id;
    shm_msgSentNum_addr = (int*) initShm(PROCESS_QUEUE_SHM_KEY_CHAR, sizeof(int), &shm_msgSentNum_key_id, &shm_msgSentNum_id);


    key_t sem_msgSentLock_key_id;
    initSem(PROCESS_QUEUE_SHM_SENT_SEM_KEY_CHAR, 1, &sem_msgSentLock_key_id, &sem_msgSentLock_id);

    key_t sem_msgRcvLock_key_id;
    initSem(PROCESS_QUEUE_SHM_RCVD_SEM_KEY_CHAR, 1, &sem_msgRcvLock_key_id, &sem_msgRcvdLock_id);


    key_t key_id;
    initMsgQ(PROCESS_QUEUE_KEY_CHAR, &key_id, &msgq_id);

}

void printEvent(Event_t event){

    printf("\n --------------------- \n");
    watch(event.entryId);
    watch(event.remaining);
    switch (event.state)
    {
    case STARTED:
        watchS("STARTED");
        break;
    case STOPPED:
        watchS("STOPPED");
        break;
    case FINISHED:
        watchS("FINISHED");
        break;
    case RESUMED:
        watchS("RESUMED");
        break;    
    }
    watch(event.time);
    watch(event.total);
    watch(event.wait);
    watch(event.arrival);
    if(event.turnaroundTime != -1){
        watch(event.turnaroundTime);
    }
    if(event.weightedTurnaroundTime != -1){

        watchF(event.weightedTurnaroundTime);

    }

    printf("\n --------------------- \n");
}

void readNewProcess() {
    //printf("\nscheduler.out -> currentTime = %d", currentTime);
    
    down(sem_msgSentLock_id);

    currentTime = getClk();
    printf("\n__Reading ( %d ) Process from PG at time %d", (*shm_msgSentNum_addr), currentTime);
    
    
    if((*shm_msgSentNum_addr) == -1) {
        endProgram = true;
    }

    for (int i = 0; i < (*shm_msgSentNum_addr); i++) {
        printf("\n+++++++++++++++++++++++++++++ For loop -> shm = %d, i = %d, time = %d", (*shm_msgSentNum_addr), i, currentTime);
        ProcessEntryMsgBuff_t message;
        int rec_val = msgrcv(msgq_id, &message, sizeof(message.p), 0, !IPC_NOWAIT);
        if (rec_val == -1)
            perror("Error in receive");
        else {
            ProcessEntry_t val = message.p;
            PCB_t new_val;

            new_val.entryId  = val.entryId;
            new_val.priority = val.priority;
            new_val.pid      = -1;
            new_val.runTime  = val.runTime;

            new_val.isRunning = false;

            new_val.arrivalTime   = val.arrival;
            new_val.startingTime  = -1;
            new_val.lastStartedTime = -1;
            new_val.remainingTime = val.runTime;

            push_PCB(&pcbHead,new_val);

            printf("\nMessage Recieved -> id = %d, arrival = %d, runtime = %d, priority = %d ---- in time = %d", val.entryId, val.arrival, val.runTime, val.priority, currentTime);
        }
    }
    printf("\n_______________Received Messages for PG %d at time %d", (*shm_msgSentNum_addr), currentTime);
    up(sem_msgRcvdLock_id);

}

