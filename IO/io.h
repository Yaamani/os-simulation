#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <libm.a>
#include "../DataStructures/linked_list.h"

int charsToInt(char*, int, int);
int getFirstIntFromString(char*, int,  int*, size_t);
ProcessEntry_t parseProcessTxtLine(char*, size_t);
void readInputFile(ProcessEntryNode_t**, ProcessEntryNode_t**);