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

void push_PCB(PCBNode_t** head, PCB_t val) {

    PCBNode_t* current = *head;

    if( *head == NULL){
        
        *head = ( PCBNode_t*) malloc(sizeof(PCBNode_t));
        (*head)->val = val;

        return;
    }

    while (current->next) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = (PCBNode_t *) malloc(sizeof(PCBNode_t));
    current->next->val = val;
    current->next->next = NULL;
}
void push_Event(EventNode_t** head, Event_t val) {

    EventNode_t* current = *head;

    if( *head == NULL){
        
        *head = ( EventNode_t*) malloc(sizeof(EventNode_t));
        (*head)->val = val;

        return;
    }

    while (current->next) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = (EventNode_t *) malloc(sizeof(EventNode_t));
    current->next->val = val;
    current->next->next = NULL;
}
void delete_PCB(PCBNode_t** head,int id){

    PCBNode_t* previous = NULL;
    PCBNode_t* current = *head;

    if((*head)->val.entryId == id){

       *head = (*head)->next; 
       free(current);
       current = NULL;
       return;
    }
    while (current) {

        if(current->val.entryId == id){

            previous->next = current->next;
            free(current);
            current = NULL;
            return;
        }
        previous = current;
        current = current->next;
    }
    
}

int get_size(PCBNode_t* head){

    int size = 0;

    PCBNode_t* current = head;

    while (current) {
        current = current->next;
        size++;
    }
    return size;
}
PCBNode_t* get_pcb(PCBNode_t* head,int id){

    PCBNode_t* current = head;

    while (current) {

        if(current->val.entryId == id){

            return current;
        }
        current = current->next;
    }
    return NULL;

}