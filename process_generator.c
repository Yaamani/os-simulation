#include "headers.h"
#include "IO/io.h"
#include "DataStructures/linked_list.h"

void clearResources(int);




ProcessEntryNode_t* processEntriesHead;

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock
    ProcessEntry_t processEntry;
    processEntry.entryId = 1;
    processEntry.arrival = 1;
    processEntry.runTime = 10;
    processEntry.priority = 5;

    processEntriesHead = createHead(processEntry);

    printf("\n%d", processEntriesHead->val.runTime);


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
