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

void push_PCB(PCBNode_t** head, PCB_t val);
void push_Event(EventNode_t** head, Event_t val);

void delete_PCB(PCBNode_t** head,int id);

int get_size(PCBNode_t* head);
PCBNode_t* get_pcb(PCBNode_t* head,int id);
