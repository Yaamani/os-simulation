#pragma once

//https://www.learn-c.org/en/Linked_lists

#include <stdlib.h>
#include "common.h"

typedef struct ProcessEntryNode {
    struct ProcessEntry val;
    struct ProcessEntryNode * next;
} ProcessEntryNode_t;

typedef struct PCBNode {
    struct PCB val;
    struct PCBNode * next;
} PCBNode_t;

typedef struct EventNode {
    struct Event val;
    struct EventNode * next;
} EventNode_t;




void push_ProcessEntry(ProcessEntryNode_t**, ProcessEntryNode_t**, ProcessEntry_t);
ProcessEntry_t removeFirst_ProcessEntry(ProcessEntryNode_t**);