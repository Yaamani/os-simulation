//https://www.learn-c.org/en/Linked_lists

#include "linked_list.h"

ProcessEntryNode_t* createHead(ProcessEntry_t val) {
    ProcessEntryNode_t* head = (ProcessEntryNode_t *) malloc(sizeof(ProcessEntryNode_t));;
    head->val = val;
    head->next = NULL;

    return head;
}

void push(ProcessEntryNode_t* head, ProcessEntry_t val) {
    ProcessEntryNode_t* current = head;
    while (current->next) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = (ProcessEntryNode_t *) malloc(sizeof(ProcessEntryNode_t));
    current->next->val = val;
    current->next->next = NULL;
}