#pragma once

//https://www.learn-c.org/en/Linked_lists

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include <math.h>

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


typedef struct MemoryNode {
    struct MemoryCell val;
    struct MemoryNode * next;
} MemoryNode_t;

typedef struct MemoryEventNode {
    struct MemoryEvent val;
    struct MemoryEventNode * next;
} MemoryEventNode_t;


// ---------start

//functions for memory

void initializeMemory(MemoryNode_t ** head);

void push_MemoryEvent(MemoryEventNode_t** head, MemoryEvent_t val);

void fixMemory(MemoryNode_t ** head);

void deallocation(MemoryNode_t ** head,MemoryNode_t * toBeDeallocatedNode);

MemoryNode_t* allocate(MemoryNode_t * head ,int entryId,int requestedSize);

MemoryNode_t* getMemoryCell(MemoryNode_t * head,int entryId);

void splitMemoreCells(MemoryNode_t * toBeSplitNode);


// ----------end 


void push_ProcessEntry(ProcessEntryNode_t**, ProcessEntryNode_t**, ProcessEntry_t);
ProcessEntry_t removeFirst_ProcessEntry(ProcessEntryNode_t**);

void push_PCB(PCBNode_t** head, PCB_t val);
void push_Event(EventNode_t** head, Event_t val);

void delete_PCB(PCBNode_t** head,int id);

int get_size(PCBNode_t* head);
PCBNode_t* get_pcb(PCBNode_t* head,int id);

void pop_push_pcb(PCBNode_t** head);