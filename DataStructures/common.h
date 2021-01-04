#pragma once

#define STARTED 0
#define STOPED 1
#define FINISHED 2
#define RESUMED 3

//#define NULL ((viod*)0)


typedef struct ProcessEntry
{
    int entryId;
    int arrival;
    int runTime;
    int priority;
} ProcessEntry_t;

typedef struct ProcessMsgBuffer
{
    long type;
    struct ProcessEntry p;
} ProcessMsgBuffer_t;




typedef struct PCB
{
    int entryId;
    int pid;
    int startingTime;
    int remainingTime;
} PCB_t;

typedef struct Event
{
    int time;
    int entryId;
    short state;
    int arrival;
    int total;
    int remaining;
    int wait;
    int turnaroundTime;
    float weightedTurnaroundTime;
} Event_t;