#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include </usr/lib/libm.a>
#include "../DataStructures/linked_list.h"

int charsToInt(char*, int, int);
int getFirstIntFromString(char*, int,  int*, size_t);
ProcessEntry_t parseProcessTxtLine(char*, size_t);
void readInputFile(ProcessEntryNode_t**, ProcessEntryNode_t**);

void writeEventsIntoFile(EventNode_t* head, float cpuUtilization, int algo);
void writeMemEventsIntoFile(MemoryEventNode_t* head);