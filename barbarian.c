#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h> 
#include <ctype.h>
#include <semaphore.h>
#include <string.h>

#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon* dungeon; // initializes the dungeon struct in this class

void barb_signal_handler(int rec_signals) { // upon receiving signal, the following actions will occur
    if (rec_signals == DUNGEON_SIGNAL){
        int healthCopy = dungeon->enemy.health; // copies the health from enemy
        dungeon->barbarian.attack = healthCopy; // places enemy's health to barbarian attack   
    }
    //if it is a semaphore signal make it hold down lever two
    else if (rec_signals == SEMAPHORE_SIGNAL){
        int treasureCnt = 4; // treasure count
        sem_t *sem_one = sem_open(dungeon_lever_one, 0);
        sem_post(sem_one); // increments the semaphore of lever one
        if (strlen(dungeon->spoils) == treasureCnt){ // checks if the len of spoils is equal to the amount of treasure
            sem_t *sem_one = sem_open(dungeon_lever_one, 0); // reopens the semaphore once the spoils are filled
        }
    }
}


int main(int argc, char *argv[]) {
    struct sigaction act; // creates sigaction struct
    act.sa_handler = &barb_signal_handler; // calls the signal handler for wizard 
    sigaction(DUNGEON_SIGNAL, &act, NULL); // changes the action of the process to dungeon signal
    sigaction(SEMAPHORE_SIGNAL, &act, NULL); // changes the action of the process to semaphore signal
    
    int fd = shm_open(dungeon_shm_name, O_RDWR, 0); // opens the shared memory segment
    // maps the shared memeory
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    while(dungeon->running){ // pauses dungeon
        pause();
    }

    munmap(dungeon, sizeof(struct Dungeon)); // unmaps the shared memory
    shm_unlink(dungeon_shm_name); // removes the shared memory

    return 0;
}