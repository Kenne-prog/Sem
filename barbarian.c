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

void barb_signal_handler(int signals) { // upon receiving signal, the following actions will occur
    if (signals == DUNGEON_SIGNAL){
        int healthCopy = dungeon->enemy.health; // copies the health from enemy
        dungeon->barbarian.attack = healthCopy; // places enemy's health to barbarian attack
    }
    else if (signals == SEMAPHORE_SIGNAL){
        sem_t *sem_one = sem_open(dungeon_lever_one, 0);
        sem_post(sem_one);
        if (strlen(dungeon->spoils) == 4){
            sem_t *sem_one = sem_open(dungeon_lever_one, 0);
        }
    }
}


int main() {

    int fd = shm_open(dungeon_shm_name, O_RDWR, 0); // opens and reads the shared memory segment
    // maps the shared memeory 
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    struct sigaction act; // creates sigaction struct
    act.sa_handler = &barb_signal_handler; // calls the signal handler for barbarian 
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(DUNGEON_SIGNAL, &act, NULL); // changes the action of the process to dungeon signal
    sigaction(SEMAPHORE_SIGNAL, &act, NULL);

    while(dungeon->running){ // pauses dungeon
        pause();
    }
    
    munmap(dungeon, sizeof(struct Dungeon)); // unmaps the shared memory
    shm_unlink(dungeon_shm_name); // removes the shared memory

    return 0;
}