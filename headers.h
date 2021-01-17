#pragma once

#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "DataStructures/linked_list.h"

 

#define true 1
#define false 0

#define SHKEY 300

#define PROCESS_QUEUE_KEY_CHAR 's'
#define PROCESS_QUEUE_SHM_KEY_CHAR 'n'
#define PROCESS_QUEUE_SHM_SENT_SEM_KEY_CHAR 'l'
#define PROCESS_QUEUE_SHM_RCVD_SEM_KEY_CHAR 'm'

#define PROCESS_FINISHES_FIRST_SEM_KEY_CHAR 'f'
//#define PROCESS_SHEDULER_PERMISSION_FIRST_SEM_KEY_CHAR 'p'
#define PROCESS_ENDED_SHM_KEY_CHAR 'e'

#define PROCESS_StartsAfterSchdulerPermission_SHM_KEY_CHAR 'p'



union semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

///==============================
//don't mess with this variable//
int * shmaddr;                 //
//===============================



int getClk()
{
    return *shmaddr;
}

 
/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{ 
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
        
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}

void initMsgQ(char c, key_t* key_id, int* msgq_id) {
    (*key_id) = ftok("keyfile", c);
    (*msgq_id) = msgget((*key_id), 0666 | IPC_CREAT);

    if ((*msgq_id) == -1)
    {
        perror("\nError in creating msgq.\n");
        exit(-1);
    }
}

void* initShm(char c, size_t size, key_t* key_id, int* shm_id) {
    (*key_id) = ftok("keyfile", c);
    (*shm_id) = shmget((*key_id), size, 0666 | IPC_CREAT);

    if ((*shm_id) == -1) {
        perror("\nError in creating shm.\n");
        exit(-1);
    }

    void* addr = shmat((*shm_id), (void *)0, 0);
    return addr;
}

void initSem(char c, int n, key_t* key_id, int* sem_id) {
    (*key_id) = ftok("keyfile", c);
    (*sem_id) = semget((*key_id), n, 0666 | IPC_CREAT);

    if ((*sem_id) == -1)
    {
        perror("\nError in creating msgq.\n");
        exit(-1);
    }
}

void setValSem(int sem_id, int i, int val) {
    union semun sem_union;
    sem_union.val = val;
    if (semctl(sem_id, i, SETVAL, sem_union) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
}


/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}


void down(int semId)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(semId, &p_op, 1) == -1)
    {
        printf("Error in down(), id = %d", semId);
        perror("Error in down()");
        exit(-1);
    }
}

void up(int semId)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(semId, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}