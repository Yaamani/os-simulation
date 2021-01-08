#include "io.h"

int charsToInt(char* line, int startIndex, int endIndex) {
    int numOfDigits = endIndex - startIndex + 1;
    int retVal = 0;
    for (int i = startIndex, j = numOfDigits-1; i <= endIndex; i++, j--) {
        retVal += (line[i] - 48) * pow(10, j);
    }
    return retVal;
}

/**
 * @brief Get the First Int From String object
 * 
 * @param line 
 * @param startIndexToSearchFrom 
 * @param len 
 * @return int 
 */
int getFirstIntFromString(char* line, int startIndexToSearchFrom, int* indexAfterFoundInt, size_t len) {
    int startIndex = -1, endIndex;
    for (int i = startIndexToSearchFrom; i < len; i++) {
        if (!(48 <= line[i] && line[i] <= 57)) { // Not a decimal digit
            if (startIndex < 0)
                startIndex = i + 1;
            else {
                endIndex = i-1;
                *indexAfterFoundInt = i;
                break;
            }
        } else if (startIndex < 0)
            startIndex = i;
    }
    return charsToInt(line, startIndex, endIndex);
}

ProcessEntry_t parseProcessTxtLine(char* line, size_t len) {
    ProcessEntry_t processEntry;
    
    int startIndexToSearchFrom = 0;
    int indexAfterFoundInt;
    for (int i = 0; i < 4; i++) {
        int num = getFirstIntFromString(line, startIndexToSearchFrom, &indexAfterFoundInt, len);
        startIndexToSearchFrom = indexAfterFoundInt;
        switch (i) {
        case 0:
            processEntry.entryId = num;
            break;
        
        case 1:
            processEntry.arrival = num;
            break;

        case 2:
            processEntry.runTime = num;
            break;

        case 3:
            processEntry.priority = num;
            break;
        }
    }

    return processEntry;
}

void readInputFile(ProcessEntryNode_t** processEntriesHead, ProcessEntryNode_t** processEntriesTail) {
    FILE * file;
    file = fopen("processes.txt", "r");
    if (file == NULL) {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
        exit(1);
    }

    ProcessEntryNode_t* tail = NULL;

    char* line = NULL;
    size_t len = 0;
    for (int i = 0; getline(&line, &len, file) != EOF; i++) {
        if (i == 0)
            continue;
        else
            push_ProcessEntry(&*processEntriesHead, &*processEntriesTail, parseProcessTxtLine(line, len));
    }

    fclose(file);
}

void writeEventsIntoFile(EventNode_t* head){

    FILE *fp;
    EventNode_t* current = head;

    float avgWTA = 0, avgWaiting = 0, stdWTA = 0;

    printf("\n START WRITING \n");
    fp = fopen("events.txt", "w+");

    float sumWTA = 0;
    float sumWaiting = 0;
    float sumSTD = 0;

    int counterWTA = 0; 

    while(current){

        char  state_string[30];    
        printf("\n LOOP \n");
        Event_t event = current->val;
        
        switch (event.state)
        {
        case STARTED:
            printf("\n go to strcpy \n");
            strcpy(state_string,"STARTED");
            printf("\n out strcpy \n");
            //watchS("STARTED");
            break;
        case STOPPED:
            strcpy(state_string,"STOPPED");
            //watchS("STOPPED");
            break;
        case FINISHED:
            printf("\n go to strcpy finished \n");
            strcpy(state_string,"FINISHED");
            printf("\n out strcpy finished \n");
            //watchS("FINISHED");
            break;
        case RESUMED:
            strcpy(state_string,"RESUMED");
            //watchS("RESUMED");
            break;    
        }
       
        fprintf(fp, "At time %d process %d %s arr %d total %d remain %d wait %d",event.time,event.entryId,state_string,event.arrival,event.total,event.remaining,event.wait);
        
        //counterWaiting++;
        if(event.turnaroundTime != -1){

            sumWaiting += event.wait;
            counterWTA++;
            sumWTA += event.weightedTurnaroundTime;
            fprintf(fp," TA %d WTA %f",event.turnaroundTime,event.weightedTurnaroundTime);
        }
        fprintf(fp,"\n\n");
        current = current->next;

    }
    printf("\n FINISHED WRITING \n");
    fclose(fp);

    fp = fopen("SchedulerPerformance.txt", "w+");

    avgWTA = sumWTA/counterWTA;
    avgWaiting = sumWaiting/counterWTA;

    current = head;

    while (current)
    {   
        Event_t event = current->val;

        if(event.turnaroundTime != -1){

            sumSTD += (event.weightedTurnaroundTime-avgWTA)*(event.weightedTurnaroundTime-avgWTA);
            
        }
        current = current->next;
    }
    stdWTA = sqrt((sumSTD/counterWTA));
        
    
    fprintf(fp,"CPU utilization = 100 \navgWTA %f \navgWaiting %f \nStdWTA %f ",avgWTA,avgWaiting,stdWTA);

    fclose(fp);

}