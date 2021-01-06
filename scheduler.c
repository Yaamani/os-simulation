#include "headers.h"
#include "IO/io.h"
#include "DataStructures/linked_list.h"

#define rr 0
#define hpf 1
#define srtf 2 

//update
#define max 1000000
PCB_t pcbs[5];
//PCB* newCommer;

//update
const int timeSahring = 5;

PCBNode_t* pcbHead = NULL;
EventNode_t* eventsHead = NULL;

int clk,chosenScheduler = 0;
int previousClk = -1;

bool endProgram = false;
bool process_ended = true;

//update
PCBNode_t* current_running = NULL; // for roundRoben

void readNewProcess(){} // yamani

int getChosenScheduler(){} //yamani

void testing(int i,int proiority,int runtime);

void runProcess(PCBNode_t* head,int id);

int hpf_run(PCBNode_t* head,bool finished);

int rr_run(bool finished);

int shortestTimeFinish_run(bool finished);

//update
void copyData(PCBNode_t* current);

void handler(int signum);

int main(int argc, char * argv[])
{

    signal(SIGUSR1,handler);

    initClk();

    int id;
    int runTime = 1 ;
    int proiority = 3;
    int i = 0;
    chosenScheduler = 1;
    //update
    current_running = (PCBNode_t*) malloc(sizeof(PCBNode_t));

    while(get_size(pcbHead) != 0 || !endProgram){

        clk = getClk();

        if(previousClk != clk){

            previousClk = clk;
            //readNewProcess();
            testing(i,proiority+2,runTime+2);
            i = i + 1;
            switch (chosenScheduler)
            {
                case rr:
                    id = rr_run(process_ended);
                    break;
                case hpf:
                    id = hpf_run(pcbHead,process_ended);
                    break;
                case srtf:
                    id = shortestTimeFinish_run(process_ended);
                    break;    
            }
            //update
            if(id != -1){
                runProcess(pcbHead,id);
            }
        }
    }

    //TODO implement the scheduler :)

    // Generate output files


    
    //upon termination release the clock resources. 
    free(current_running); //be sure delete by & or not
    current_running = NULL;
    destroyClk(true);
}

int hpf_run(PCBNode_t* head,bool finished){

    if(head == NULL || !finished){

        return -1;
    }
    PCBNode_t* current = head; 
    //PCBNode_t* highest_pcb
    int highest_p = -1;
    int id;

    while(current){

        if(current->val.priority > highest_p){

            highest_p = current->val.priority;
            id = current->val.entryId;
        }
        current = current->next;
    }
    finished = false;
    return id;
}

//update

int rr_run(bool finished){

    int id = -1;
    if(pcbHead == NULL){

        return -1;
    }
    if(pcbHead->next == NULL){

        //current_running->val.isRunning = pcbHead->val.isRunning;
        copyData(pcbHead); 
        if(pcbHead->val.isRunning){
            return -1;
        }
        return pcbHead->val.entryId;  
    }
    //PCBNode_t* current = pcbHead;
    if(!finished){
        
        /*while(current){

            if(current->val.isRunning){

                if( clk - current->val.lastStartedTime > timeSahring){
                    
                    if(current->next != NULL){
                        return current->next->val.entryId;
                    }
                    else{return -1;}
                }
                else{
                    return -1;
                }

            }
        }*/
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

int shortestTimeFinish_run(bool finished){

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
        if(remainingTime  < shortest_Remainig_Time){

            shortest_Remainig_Time = current->val.remainingTime;
            id = current->val.entryId;
        }
    }
    if(current->val.isRunning){
        return -1;
    }
    return id; 
}

void runProcess(PCBNode_t* head,int id){

    if(id == -1){
        return;
    }

    PCBNode_t* current = head; 
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
           
            if(!process_ended){

                event.turnaroundTime = -1;
                event.weightedTurnaroundTime = -1;
                event.state = STOPPED;

                kill(current->val.pid,SIGSTOP);
                current->val.isRunning = false;
            }
            else{
                event.state = FINISHED;
                event.turnaroundTime = clk - current->val.arrivalTime;
                event.weightedTurnaroundTime = (float)event.turnaroundTime / current->val.runTime;
                delete_PCB(&head,id);
            }
            push_Event(&eventsHead,event);
            break;    
        }
        current = current->next;

    }
    current = get_pcb(head,id);

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
            execl("process.out","process.out",current->val.remainingTime,NULL);

        }

        current->val.pid = pid;

    }
    push_Event(&eventsHead,event);
    current->val.isRunning = true;  

}

//update
void copyData(PCBNode_t* current){

    current_running->val.isRunning = current->val.isRunning;
    current_running->next = current->next;
    current_running->val.entryId = current->val.entryId;
}

void handler(int signum){

    process_ended = true;
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