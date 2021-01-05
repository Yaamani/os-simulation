#include "headers.h"
#include "IO/io.h"


//PCB pcbs[];
//PCB* newCommer;


int main(int argc, char * argv[])
{
    initClk();
    
    //TODO implement the scheduler :)
    
    // if (chosenScheduler == rr) {
    //     rr_run(pcbs, newCommer);
    // } else if (hosenScheduler == hpf) {
    //     hpf_run(pcbs, newCommer);
    // }


    // Generate output files


    
    //upon termination release the clock resources.    
    destroyClk(true);
}
