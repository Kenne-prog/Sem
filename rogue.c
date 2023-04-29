#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <semaphore.h>
#include <string.h>

#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon* dungeon; // initializes the dungeon struct in this class

void rogue_signal_handler(int signals) {
    if (signals == DUNGEON_SIGNAL){
        float min = 0;
        float max = 100;
        float mid = ceil((min + max) / 2);
        bool picked = false;

        while (!picked && (max - min) > LOCK_THRESHOLD) {
            dungeon->rogue.pick = mid;
            usleep(50000);

            if (dungeon->trap.locked) {
            if (dungeon->trap.direction == 'u') {
                    min = mid;
                } else {
                    max = mid;
                }
                mid = ceil((min + max) / 2);
            } else if (dungeon->trap.direction == '-') {
                picked = true;
            }
        }
    }
    else if (signals == SEMAPHORE_SIGNAL){
    sem_t *sem_one = sem_open(dungeon_lever_one, 0);
    sem_t *sem_two = sem_open(dungeon_lever_two, 0);


    sem_wait(sem_one);
    sem_wait(sem_two);

    while (strlen(dungeon->spoils) <= 4) {
        dungeon->spoils[0] = dungeon->treasure[0];
        dungeon->spoils[1] = dungeon->treasure[1];
        dungeon->spoils[2] = dungeon->treasure[2];
        dungeon->spoils[3] = dungeon->treasure[3]; 
        usleep(10000);
    }
    
    dungeon->treasure[4] = '\0';
    dungeon->spoils[4] = '\0';
    
    sem_post(sem_one);
    sem_post(sem_two);
    }
}


int main() {
    int fd = shm_open(dungeon_shm_name, O_RDWR, 0); // opens the shared memory segment
    // maps the shared memeory 
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    struct sigaction act;
    act.sa_handler = &rogue_signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(DUNGEON_SIGNAL, &act, NULL);
    sigaction(SEMAPHORE_SIGNAL, &act, NULL);

    sleep(1);

    while(dungeon->running){
        pause();
    }
    
    munmap(dungeon, sizeof(struct Dungeon)); // unmaps the shared memory
    shm_unlink(dungeon_shm_name); // removes the shared memory
       
    return 0;
}