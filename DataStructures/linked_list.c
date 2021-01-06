//https://www.learn-c.org/en/Linked_lists

#include "linked_list.h"

void push_ProcessEntry(ProcessEntryNode_t** head, ProcessEntryNode_t** tail, ProcessEntry_t val) {
    if ((*head) == NULL) {
        (*head) = (*tail) = (ProcessEntryNode_t *) malloc(sizeof(ProcessEntryNode_t));
        
        (*head)->val = val;
        (*head)->next = NULL;
        return;
    }

    (*tail)->next = (ProcessEntryNode_t *) malloc(sizeof(ProcessEntryNode_t));
    (*tail)->next->val = val;
    (*tail)->next->next = NULL;

    (*tail) = (*tail)->next;
}

ProcessEntry_t removeFirst_ProcessEntry(ProcessEntryNode_t** head) {
    ProcessEntry_t retVal;
    if ((*head) == NULL) {
        retVal.entryId = -1;
        retVal.arrival = -1;
        retVal.runTime = -1;
        retVal.priority = -1;
        return retVal;
    }

    ProcessEntryNode_t* next_node = (*head)->next;
    retVal = (*head)->val;
    free(*head);
    *head = next_node;
    return retVal;
} 