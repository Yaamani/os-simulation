# OS Simulator 

## Introduction  

A CPU scheduler determines an order for the execution of its scheduled processes; it
decides which process will run according to a certain data structure that keeps track
of the processes in the system and their status.
A process, upon creation, has one of the three states: Running, Ready, Blocked (doing
I/O, using other resources than CPU or waiting on unavailable resource).
A bad scheduler will make a very bad operating system, so your scheduler should be
as much optimized as possible in terms of memory and time usage.
## System Description
Consider a Computer with 1-CPU and innite memory. It is required to make a
scheduler with its complementary components as sketched in the following diagrams.
![image](https://user-images.githubusercontent.com/46090694/117367331-55b8e080-aec2-11eb-9b13-1d539979ae74.png)

### Part I: Process Generator (Simulation & IPC)
Code File process generator.c
The process generator should do the following tasks...
 Read the input (check the input/output section below).
 Ask the user for the chosen scheduling algorithm and its parameters, if there
are any.
 Initiate and create the scheduler and clock processes.
 Create a data structure for processes and provide it with its parameters.
 Send the information to the scheduler at the appropriate time (when a process
arrives), so that it will be put it in its turn.
 At the end, clear IPC resources.
### Part II: Clock (Simulation & IPC)
Code File clk.c
The clock module is used to emulate an integer time clock. This module is already
built for you.
### Part III: Scheduler (OS Design & IPC)
Code File scheduler.c
The scheduler is the core of your work, it should keep track of the processes and their
states and it decides - based on the used algorithm - which process will run and for
how long.
You are required to implement the following THREE algorithms:
1. Non-preemptive Highest Priority First (HPF).
2. Shortest Remaining time Next (SRTN).
3. Round Robin (RR).

The scheduling algorithm only works on the processes in the ready queue. (Processes
that have already arrived.)
The scheduler should be able to
1. Start a new process. (Fork it and give it its parameters.)
2. Switch between two processes according to the scheduling algorithm. (Stop the
old process and save its state and start/resume another one.)
3. Keep a process control block (PCB) for each process in the system. A PCB
should keep track of the state of a process; running/waiting, execution time,
remaining time, waiting time, etc.
4. Delete the data of a process when it gets notices that it finished. When a
process finishes it should notify the scheduler on termination, the scheduler
does NOT terminate the process.
5. Report the following information
(a) CPU utilization.
(b) Average weighted turnaround time.
(c) Average waiting time.
(d) Standard deviation for average weighted turnaround time.
6. Generate two les: (check the input/output section below)
(a) Scheduler.log
(b) Scheduler.perf

### Part IV: Process (Simulation & IPC)
Code File process.c
Each process should act as if it is CPU-bound.
Again, when a process finishes it should notify the scheduler on termination, the
scheduler does NOT terminate the process.

### Part V: Input/Output (Simulation & OS Design Evaluation)
#### Input File

Comments are added as lines beginning with # and should be ignored.
 Each non-comment line represents a process.
 Fields are separated with one tab character.
 You can assume that processes are sorted by their arrival time. Take care that
2 or more processes may arrive at the same time.
 You can use the test generator.c to generate a random test case.

#### Output Files

 Comments are added as lines beginning with # and should be ignored.
 Approximate numbers to the nearest 2 decimal places
 Allowed states: started, resumed, stopped, finished.
 TA & WTA are written only at finished state.
 You need to stick to the given format because files are compared automatically.


 
