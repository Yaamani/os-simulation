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

void pop_push_pcb(PCBNode_t** head){
    PCBNode_t* current = *head;
    if(current->next==NULL){
        return;
    }
    
    //get to last element
    while (current->next) {
        current = current->next;
    }
    current->next =*head;
    *head = (*head)->next;
    current->next->next=NULL;
}

void delete_PCB(PCBNode_t** head,int id){

    PCBNode_t* previous = NULL;
    PCBNode_t* current  = *head;

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

// start

// functions of memory

void push_MemoryEvent(MemoryEventNode_t** head, MemoryEvent_t val) {

    MemoryEventNode_t* current = *head;

    if( *head == NULL){
        
        *head = ( MemoryEventNode_t*) malloc(sizeof(MemoryEventNode_t));
        (*head)->val = val;

        return;
    }

    while (current->next) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = (MemoryEventNode_t *) malloc(sizeof(MemoryEventNode_t));
    current->next->val = val;
    current->next->next = NULL;
}

void initializeMemory(MemoryNode_t ** head){

    *head = (MemoryNode_t *) malloc(sizeof(MemoryNode_t));

    (*head)->val.size = 1024;
    (*head)->val.entryId = -1;
    (*head)->val.requestedSize = 0;
    (*head)->val.startAddress = 0;
}

MemoryNode_t* getMemoryCell(MemoryNode_t * head,int entryId){

    MemoryNode_t* current = head;

    while (current){
        if(current->val.entryId==entryId){
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}


void splitMemoreCells(MemoryNode_t * toBeSplitNode){

    MemoryCell_t val;
    toBeSplitNode->val.size = toBeSplitNode->val.size/2;

    val.size = toBeSplitNode->val.size;
    val.entryId = -1;
    val.startAddress = toBeSplitNode->val.startAddress + toBeSplitNode->val.size;
    val.requestedSize = 0;

    
    MemoryNode_t* newNode = (MemoryNode_t *) malloc(sizeof(MemoryNode_t));
    newNode->val = val;
    newNode->next = toBeSplitNode->next;
    toBeSplitNode->next = newNode;

    
    //return toBeSplitNode;
}


int get_size_memory(MemoryNode_t* head){

    int size = 0;

    MemoryNode_t* current = head;

    while (current) {
        current = current->next;
        size++;
    }
    return size;
}


MemoryNode_t* allocate(MemoryNode_t * head ,int entryId,int requestedSize){
    printf("\n------------------------allocating %d ",entryId);
    MemoryNode_t* targetNode = NULL;
    MemoryNode_t * current =  head;
    printf("\n --------------- size before allocation : %d",get_size_memory(head));   
    int n = ceil(log(requestedSize)/log(2));

    int memoryCellSize = pow(2,n);
    
    while(current){
        if(current->val.entryId==-1 && current->val.size>=requestedSize){
            if(targetNode==NULL){
                targetNode = current;
            }
            else{
                if(current->val.size < targetNode->val.size){
                    targetNode =current;
                }    
            }
        }
        current = current->next;
    }
    if(targetNode == NULL){
        return NULL;
    }
    while(memoryCellSize != targetNode->val.size ){

        splitMemoreCells(targetNode);

    }
    targetNode->val.entryId = entryId;
    targetNode->val.requestedSize = requestedSize;

    printf("\n --------------- size after allocation : %d",get_size_memory(head));   
    printf("\nprocess id %d reserved %d",entryId,targetNode->val.size);
    
    return targetNode;

}

void deallocation(MemoryNode_t ** head,MemoryNode_t * toBeDeallocatedNode){
    printf("\n------------deallocating %d ",toBeDeallocatedNode->val.entryId);
    
    printf("\n --------------- size before deallocation : %d",get_size_memory(*head));   
    toBeDeallocatedNode->val.entryId = -1;
    fixMemory(head);
    printf("\n --------------- size after deallocation : %d",get_size_memory(*head));   
}

void fixMemory(MemoryNode_t ** head){

    MemoryNode_t* current = *head;

    if(current == NULL){
        return;
    }
    
    while (current->next) {
        
        MemoryCell_t currentCell = current->val;
        MemoryCell_t nextCell = current->next->val;
        
        if(currentCell.entryId == -1 && nextCell.entryId == -1 && nextCell.size == currentCell.size && currentCell.startAddress%(currentCell.size*2)==0 ){

            MemoryNode_t* temp  = current->next;
            current->next = current->next->next;
            current->val.size = current->val.size * 2;
            free(temp);    
            continue;
        }

        current = current->next;
    }

}

// end
