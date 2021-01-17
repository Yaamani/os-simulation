#include "headers.h"
#include "IO/io.h"
#include "DataStructures/linked_list.h"

#define rr 0
#define hpf 1
#define srtf 2 

#define watch(x) printf(#x ": %d \n", x)
#define watchF(x) printf(#x ": %f \n", x)
#define watchS(x) printf( #x ": %s \n", x)

//update
#define max 1000000

//update
const int timeSharing = 3;

bool thereIsAProcessRunning = false;

PCBNode_t* pcbHead = NULL;
EventNode_t* eventsHead = NULL;
MemoryEventNode_t* memoryEventsHead = NULL;
MemoryNode_t * memHead = NULL;
PCBNode_t* memOverflowHead = NULL;

int clk,chosenScheduler;
int previousClk = 0;

int schedulerRunTime = 0;
int processesRunTime = 0;

bool endProgram = false;
bool process_ended = false;

//update
PCBNode_t* current_running = NULL; // for roundRoben

void readNewProcess(); // yamani

 void addPreviouslyUnfitProcess();
// int getChosenScheduler(){} //yamani

void testing(int i,int proiority,int runtime);

void runProcess(int id);

int hpf_run();

int rr_run();

int shortestTimeFinish_run();

void set_semaphores_for_process(int finishesFirst,int permission);
//update
void copyData(PCBNode_t* current);

void handler(int signum);

void printEvents();

void printEvent(Event_t event);

void IPCs();

void destroyIPCs();

void handleFinished();

int shm_msgSentNum_id;
int* shm_msgSentNum_addr;
int sem_msgSentLock_id;
int sem_msgRcvdLock_id;
int msgq_id;

int shm_processStartsAfterSchdulerPermission_id;
int* shm_processStartsAfterSchdulerPermission_addr;

int shm_processEnded_id;
int* shm_processEnded_addr;

int sem_processFinishesFirstLock_id;

//int sem_processStartsAfterSchdulerPermission_id;



int currentTime;


int main(int argc, char * argv[])
{
    
    chosenScheduler  = argv[1][0]-'0';
    printf("\n shceduler.c -> chosenAlgorithm = %d \n", chosenScheduler);
    /*if(chosenScheduler == rr){
        process_ended =false;
    }*/
    signal(SIGUSR1,handler);

    IPCs();

    initClk();

    initializeMemory(&memHead);

    //currentTime = getClk();

    int id;

    //update
    current_running = (PCBNode_t*) malloc(sizeof(PCBNode_t));

    while(get_size(pcbHead) != 0 || !endProgram){

        clk = getClk();
        
        if(previousClk != clk){
            //printf("\n ***** UPPING SEMAPHORE **** \n");
            //up(sem_processStartsAfterSchdulerPermission_id); 
            *shm_processStartsAfterSchdulerPermission_addr = 1;
            schedulerRunTime++;
            
            printf("\n ***** DOWNING SEMAPHORE **** \n");
            down(sem_processFinishesFirstLock_id);
            printf("\n ***** DOWNED **** \n");

            if (*shm_processEnded_addr == 1) { // true
                process_ended = true;
                *shm_processEnded_addr = 0;
            }

            //sleep(0.6);
            //printf("\n XXXXXXXsize of PCB = %d, end program value = %d \n", get_size(pcbHead), endProgram);
            previousClk = clk;
            //printf("\n ******* Going to Read New Procs at time %d \n", getClk());
            

            handleFinished();

            readNewProcess();

            
            switch (chosenScheduler)
            {
                case rr:
                    id = rr_run();
                    //printf("\n process id %d that will run \n",id);
                    break;
                case hpf:
                    id = hpf_run();
                    //printf("\n process id %d that will run \n",id);
                    break;
                case srtf:
                    id = shortestTimeFinish_run();
                    break;    
            }
            //update
            //if(id != -1){
            runProcess(id);
            //}
            if(get_size(pcbHead)!= 0){

                processesRunTime++;
            }
            if(!thereIsAProcessRunning){
                set_semaphores_for_process(1,0);
            }
        }
       
       
    }

    // Generate output files
   
    
    //upon termination release the clock resources. 
    
    //printEvents();

    destroyIPCs();

    //printf("\n Scheduler is Terminated \n");
    float cpuUtilization = 100-(((float)(schedulerRunTime-processesRunTime))/getClk())*100;
    //printf("",cpuUtilization)
    writeEventsIntoFile(eventsHead, cpuUtilization, chosenScheduler);
    writeMemEventsIntoFile(memoryEventsHead);

    free(current_running); //be sure delete by & or not
    current_running = NULL;
    
    destroyClk(true /*false*/);
}

int hpf_run(){

    if(pcbHead == NULL || thereIsAProcessRunning){
        return -1;
    }
    PCBNode_t* current = pcbHead; 
    
    int highest_p = -1;
    int id=-1;

    while(current){
        if(!current->val.isRunning){
            if(current->val.priority > highest_p){

                highest_p = current->val.priority;
                id = current->val.entryId;
            }
        }    
        current = current->next;
    }
    
    return id;
}

//update

void set_semaphores_for_process(int finishesFirst,int permission){
    setValSem(sem_processFinishesFirstLock_id, 0, finishesFirst);
    *shm_processStartsAfterSchdulerPermission_addr  = permission;
    //setValSem(sem_processStartsAfterSchdulerPermission_id, 0, permission);
}

int rr_run(){


    if(pcbHead == NULL){
        return -1;
    }
    //PCBNode_t* current = pcbHead;
    //printf("\n INSIDE rr_run \n");
    if(!thereIsAProcessRunning){
        return pcbHead->val.entryId; // for first process in program
/*
        if(pcbHead->val.isRunning){

            pop_push_pcb(&pcbHead);
            if(pcbHead->next != NULL){
                return pcbHead->val.entryId; // get the next process
            }
            else
            {
                return -1; // last process so should return -1
            }

        }
        else 
        {
            return pcbHead->val.entryId; // for first process in program
        }
*/        
    }
    else
    {

        if(pcbHead->val.isRunning){

            if(clk - pcbHead->val.lastStartedTime >= timeSharing){ 
                pop_push_pcb(&pcbHead);
                if(pcbHead->next == NULL){
                    return -1;
                }
                return pcbHead->val.entryId;
            }
            else{
                return -1;
            }
        }
        else 
        {
        
            return pcbHead->val.entryId; // i was paused
        }

    }

}

//update

int shortestTimeFinish_run(){

    if(pcbHead == NULL){
        return -1;
    }
    int shortest_Remainig_Time = max;
    int id = -1;
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

            shortest_Remainig_Time = remainingTime;//current->val.remainingTime;
            id = current->val.entryId;
        }
        current = current->next;
    }
    if(id ==-1){
        return -1;
    }
    if(get_pcb(pcbHead,id)->val.isRunning){
        return -1;
    }
    return id; 
}

void runProcess(int id){

    PCBNode_t* current = pcbHead; 
    Event_t event;
/*
    if(id == -1){
        if(process_ended){
            while(current){
                //printf("\n INSIDE PROCESS ENDED INSIDE RUNPROCESS \n");
                if(current->val.isRunning){
                    
                    //add new event
                    current->val.remainingTime =  current->val.remainingTime - (clk - current->val.lastStartedTime);

                    event.entryId = current->val.entryId;
                    event.remaining =  current->val.remainingTime;
                    event.time = clk;
                    event.total = current->val.runTime;
                    event.arrival = current->val.arrivalTime;
                    event.wait = (clk - current->val.arrivalTime) - (current->val.runTime-current->val.remainingTime);
                
            
                    event.state = FINISHED;
                    event.turnaroundTime = clk - current->val.arrivalTime;
                    event.weightedTurnaroundTime = (float)event.turnaroundTime / current->val.runTime;
                    
                    MemoryNode_t * toBeDeletedNode = getMemoryCell(memHead,event.entryId);
                    MemoryEvent_t newMemEvent ;

                    newMemEvent.entryId = current->val.entryId ;
                    newMemEvent.time = getClk();
                    newMemEvent.allocated = false;
                    newMemEvent.requestedSize = current->val.memorySize;
                    newMemEvent.startAddress = toBeDeletedNode->val.startAddress ;
                    newMemEvent.endAddress = toBeDeletedNode->val.startAddress + toBeDeletedNode->val.size - 1;
                    push_MemoryEvent(&memoryEventsHead,newMemEvent);
                
                    deallocation(&memHead,toBeDeletedNode);
                    
                    delete_PCB(&pcbHead,current->val.entryId);
                    //printf("\n deleted  %d \n",pcbHead->val.entryId);
                    addPreviouslyUnfitProcess();
                    
                    push_Event(&eventsHead,event);
                    printEvent(event);
                    thereIsAProcessRunning = false;
                    break;    
                }
                current = current->next;

            }
            process_ended= false; 

        }
        return;
    }
*/  
    if(id == -1){
        return;
    }

    while(current){
        //printf("\n INSIDE RUN PROCESS \n");
        if(current->val.isRunning){
            
            //add new event
            current->val.remainingTime =  current->val.remainingTime - (clk - current->val.lastStartedTime);

            event.entryId = current->val.entryId;
            event.remaining =  current->val.remainingTime;
            event.time = clk;
            event.total = current->val.runTime;
            event.arrival = current->val.arrivalTime;
            event.wait = (clk - current->val.arrivalTime) - (current->val.runTime-current->val.remainingTime);
            thereIsAProcessRunning = false;
            //if(!process_ended){

            event.turnaroundTime = -1;
            event.weightedTurnaroundTime = -1;
            event.state = STOPPED;

            kill(current->val.pid,SIGSTOP);
            current->val.isRunning = false;
            //}
            /*else {

                MemoryNode_t * toBeDeletedNode = getMemoryCell(memHead,event.entryId);
                MemoryEvent_t newMemEvent;
                
                newMemEvent.entryId = current->val.entryId ;
                newMemEvent.time = getClk();
                newMemEvent.allocated = false;
                newMemEvent.requestedSize = current->val.memorySize;
                newMemEvent.startAddress = toBeDeletedNode->val.startAddress ;
                newMemEvent.endAddress = toBeDeletedNode->val.startAddress + toBeDeletedNode->val.size - 1;
                push_MemoryEvent(&memoryEventsHead,newMemEvent);
            
                deallocation(&memHead,toBeDeletedNode);
                addPreviouslyUnfitProcess();
                event.state = FINISHED;
                event.turnaroundTime = clk - current->val.arrivalTime;
                event.weightedTurnaroundTime = (float)event.turnaroundTime / current->val.runTime;
                delete_PCB(&pcbHead,current->val.entryId);
            //    printf("\n deleted  %d \n",pcbHead->val.entryId);
            }*/
            
            push_Event(&eventsHead,event);
            printEvent(event);
            
            break;    
        }
        current = current->next;

    }

    //printf("\n ### pcb %d is going to create \n",id);
    
    current = get_pcb(pcbHead,id);
    current_running = current ;// copyData(current); // make
    //printf("\n entry id :::: %d \n",current->val.entryId);
    event.entryId = current->val.entryId;
    event.remaining =  current->val.remainingTime;
    event.time = clk;
    event.total = current->val.runTime;
    event.arrival = current->val.arrivalTime;
    event.wait = (clk - current->val.arrivalTime) - (current->val.runTime-current->val.remainingTime);
    event.turnaroundTime = -1;
    event.weightedTurnaroundTime = -1;
    set_semaphores_for_process(0,0);// added
    thereIsAProcessRunning = true;
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
      //      printf("\n CREATE PROCESS id = %d \n",id);
            
            int timeString = current->val.remainingTime;
            int length = snprintf( NULL, 0, "%d", timeString );
            char* str = malloc( length + 1 );
            snprintf( str, length + 1, "%d", timeString );
            
            execl("process.out","process.out", str , (char*) NULL);

        }

        current->val.pid = pid;

    }

    push_Event(&eventsHead,event);
    printEvent(event);
    current->val.isRunning = true; 
    //process_ended= false; 

}

//update
void copyData(PCBNode_t* current){

    //current_running->val.isRunning = current->val.isRunning;
    current_running->next = current->next;
    current_running->val =  current->val;
    //current_running->val.entryId = current->val.entryId;
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

        printf("\n Error in Print Events\n");
        printf("\n There is no events\n\n");

        return;
    }
    int i = 1;
    while(current){

        //printf("\n Event%d \n",i);
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
        printf("\n -------------------------\n");
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


    key_t sem_processFinishesFirstLock_key_id;
    initSem(PROCESS_FINISHES_FIRST_SEM_KEY_CHAR, 1, &sem_processFinishesFirstLock_key_id, &sem_processFinishesFirstLock_id);
    setValSem(sem_processFinishesFirstLock_id, 0, 1);
    //printf("sem_processFinishesFirstLock_id = %d",sem_processFinishesFirstLock_id);


    key_t shm_processEnded_key_id;
    shm_processEnded_addr = (int*) initShm(PROCESS_ENDED_SHM_KEY_CHAR, sizeof(int), &shm_processEnded_key_id, &shm_processEnded_id);
    *shm_processEnded_addr = 0;


    key_t shm_processStartsAfterSchdulerPermission_key_id;
    shm_processStartsAfterSchdulerPermission_addr = (int*) initShm(PROCESS_StartsAfterSchdulerPermission_SHM_KEY_CHAR, sizeof(int), &shm_processStartsAfterSchdulerPermission_key_id, &shm_processStartsAfterSchdulerPermission_id);
    *shm_processStartsAfterSchdulerPermission_addr = 0;

    //key_t sem_processStartsAfterSchdulerPermission_key_id;
    //initSem(PROCESS_SHEDULER_PERMISSION_FIRST_SEM_KEY_CHAR, 1, &sem_processStartsAfterSchdulerPermission_key_id, &sem_processStartsAfterSchdulerPermission_id);
    //setValSem(sem_processStartsAfterSchdulerPermission_id, 0, 0);
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


void addPreviouslyUnfitProcess(){

    PCBNode_t* current = memOverflowHead;
    while(current){
        PCB_t new_val = current->val;
        MemoryNode_t * newlyAllocated = allocate(memHead,new_val.entryId,new_val.memorySize);

        if(newlyAllocated != NULL){
            MemoryEvent_t newMemEvent ;
            newMemEvent.entryId = new_val.entryId ;
            newMemEvent.time = getClk();
            newMemEvent.allocated = true;
            newMemEvent.requestedSize = new_val.memorySize;
            newMemEvent.startAddress = newlyAllocated->val.startAddress ;
            newMemEvent.endAddress = newlyAllocated->val.startAddress + newlyAllocated->val.size - 1;

            push_MemoryEvent(&memoryEventsHead,newMemEvent);
            push_PCB(&pcbHead,new_val);
            current = current->next;
            delete_PCB(&memOverflowHead,new_val.entryId);
            continue;
        }
        current= current->next;
    }
}

void readNewProcess() {
    //printf("\nscheduler.out -> currentTime = %d", currentTime);
    if(endProgram){
        return;
    }
    //printf("\n INSIDE readNewProcess \n");
    down(sem_msgSentLock_id);

    currentTime = getClk();
    //printf("\n __Reading ( %d ) Process from PG at time %d \n", (*shm_msgSentNum_addr), currentTime);
    
    
    if((*shm_msgSentNum_addr) == -1) {
        endProgram = true;
    }

    for (int i = 0; i < (*shm_msgSentNum_addr); i++) {
        //printf("\n +++++++++++++++++++++++++++++ For loop -> shm = %d, i = %d, time = %d \n", (*shm_msgSentNum_addr), i, currentTime);
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
            new_val.memorySize = val.memorySize;
            new_val.isRunning = false;
            new_val.arrivalTime   = val.arrival;
            new_val.startingTime  = -1;
            new_val.lastStartedTime = -1;
            new_val.remainingTime = val.runTime;
            
            MemoryNode_t * newlyAllocated = allocate(memHead,new_val.entryId,new_val.memorySize);
            
            if(newlyAllocated == NULL){
                // todo handlha
                push_PCB(&memOverflowHead,new_val);
            }
            else{
                MemoryEvent_t newMemEvent ;
                newMemEvent.entryId = new_val.entryId ;
                newMemEvent.time = getClk();
                newMemEvent.allocated = true;
                newMemEvent.requestedSize = new_val.memorySize;
                newMemEvent.startAddress = newlyAllocated->val.startAddress ;
                newMemEvent.endAddress = newlyAllocated->val.startAddress + newlyAllocated->val.size - 1;

                push_MemoryEvent(&memoryEventsHead,newMemEvent);
                push_PCB(&pcbHead,new_val);
            }
            

            //printf("\n Message Recieved -> id = %d, arrival = %d, runtime = %d, priority = %d ---- in time = %d \n", val.entryId, val.arrival, val.runTime, val.priority, currentTime);
        }
    }
    //printf("\n _______________Received Messages for PG %d at time %d \n", (*shm_msgSentNum_addr), currentTime);
    up(sem_msgRcvdLock_id);

}

void destroyIPCs() {
    if(semctl(sem_processFinishesFirstLock_id,0,IPC_RMID) == -1){

        printf("Error in remove, id = %d", sem_processFinishesFirstLock_id);
        perror("Error in remove");
    }

    if(semctl(sem_msgSentLock_id,0,IPC_RMID) == -1){

        printf("Error in remove, id = %d", sem_msgSentLock_id);
        perror("Error in remove");
    }

    if(semctl(sem_msgRcvdLock_id,0,IPC_RMID) == -1){

        printf("Error in remove, id = %d", sem_msgRcvdLock_id);
        perror("Error in remove");
    }

    if(msgctl(msgq_id, IPC_RMID, (struct msqid_ds *) 0) == -1) {
        printf("Error in remove, id = %d", msgq_id);
        perror("Error in remove");
    }

    shmdt(shm_msgSentNum_addr);
    shmctl(shm_msgSentNum_id, IPC_RMID, NULL);
    
    shmdt(shm_processEnded_addr);
    shmctl(shm_processEnded_id, IPC_RMID, NULL);

    shmdt(shm_processStartsAfterSchdulerPermission_addr);
    shmctl(shm_processStartsAfterSchdulerPermission_id, IPC_RMID, NULL);
}

void handleFinished(){

    if(process_ended && current_running != NULL){

        PCBNode_t* current = current_running; 
        Event_t event;

        current->val.remainingTime =  current->val.remainingTime - (clk - current->val.lastStartedTime);

        event.entryId = current->val.entryId;
        event.remaining =  current->val.remainingTime;
        event.time = clk;
        event.total = current->val.runTime;
        event.arrival = current->val.arrivalTime;
        event.wait = (clk - current->val.arrivalTime) - (current->val.runTime-current->val.remainingTime);
    

        event.state = FINISHED;
        event.turnaroundTime = clk - current->val.arrivalTime;
        event.weightedTurnaroundTime = (float)event.turnaroundTime / current->val.runTime;
        
        MemoryNode_t * toBeDeletedNode = getMemoryCell(memHead,event.entryId);
        MemoryEvent_t newMemEvent ;

        newMemEvent.entryId = current->val.entryId ;
        newMemEvent.time = getClk();
        newMemEvent.allocated = false;
        newMemEvent.requestedSize = current->val.memorySize;
        newMemEvent.startAddress = toBeDeletedNode->val.startAddress ;
        newMemEvent.endAddress = toBeDeletedNode->val.startAddress + toBeDeletedNode->val.size - 1;
        push_MemoryEvent(&memoryEventsHead,newMemEvent);
    
        deallocation(&memHead,toBeDeletedNode);
        
        delete_PCB(&pcbHead,current->val.entryId);
        //printf("\n deleted  %d \n",pcbHead->val.entryId);
        addPreviouslyUnfitProcess();
        
        push_Event(&eventsHead,event);
        printEvent(event);
        thereIsAProcessRunning = false;
        process_ended = false;       
    }

}