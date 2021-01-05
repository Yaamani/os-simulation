#include "headers.h"
#include "IO/io.h"
#include "DataStructures/linked_list.h"

void clearResources(int);



ProcessEntryNode_t* processEntriesHead = NULL;

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    readInputFile(&processEntriesHead);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.

    // int chosenAlgorithm;
    // printf("\nPlease Enter Scheduling Algorithm (0:hpf, 1:srtn, 2:rr): ");
    // scanf("%d", &chosenAlgorithm);
    // printf("\nchosenAlgorithm = %d\n", chosenAlgorithm);

    // 3. Initiate and create the scheduler and clock processes.
    int pid;
    for (int i = 0; i < 2; i++) {
        pid = fork();
        if (pid == 0) { // Child
            if (i == 0) {
                printf("\nI'm now child.\n");
                char *cwd;
                cwd=malloc(255);
                getcwd(cwd,255);
                strcat(cwd,"/");
                //execl("/Users/mac/Desktop/College/OS/Project/os-project", "./clk.out", (char*) NULL);
                execl("/bin/ps", "ps", "-l", (char*) NULL);
            }
        }
    }
    // 4. Use this function after creating the clock process to initialize clock

    // ProcessEntryNode_t* current = processEntriesHead;
    // while (current)
    // {
    //     printf("\n%d\t%d\t%d\t%d", current->val.entryId, current->val.arrival, current->val.runTime, current->val.priority);
    //     current = current->next;
    // }
    
    
    

    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
