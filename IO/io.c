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
