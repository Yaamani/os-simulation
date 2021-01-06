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
#include <string.h>

int main() {
    int pid = fork();
    if (pid == 0) {
        printf("\nI'm child.....\n");
        execl("/bin/ls", "ls", NULL);
    } else {
        printf("\nI'm parent.....\n");
    }
    return 0;
}