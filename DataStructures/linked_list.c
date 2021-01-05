//https://www.learn-c.org/en/Linked_lists

#include "linked_list.h"

void push_ProcessEntry(ProcessEntryNode_t** head, ProcessEntry_t val) {
    if ((*head) == NULL) {
        (*head) = (ProcessEntryNode_t *) malloc(sizeof(ProcessEntryNode_t));
        (*head)->val = val;
        (*head)->next = NULL;
        return;
    }

    ProcessEntryNode_t* current = *head;
    while (current->next) {
        current = current->next;
    }

    current->next = (ProcessEntryNode_t *) malloc(sizeof(ProcessEntryNode_t));
    current->next->val = val;
    current->next->next = NULL;
    
}