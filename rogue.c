#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <semaphore.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon* dungeon;


// Binary search function to pick the lock
void signal_handler(int signal) {
    if (signal == DUNGEON_SIGNAL){
        float min = 0;
        float max = 100;
        float mid = ceil((min + max) / 2);
        bool picked = false;

        while (!picked && (max - min) > LOCK_THRESHOLD) {
            dungeon->rogue.pick = mid;
            usleep(20000);

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
    else if (signal == SEMAPHORE_SIGNAL){
    sem_t *door_sem_1 = sem_open("/LeverOne", 0);
    sem_t *door_sem_2 = sem_open("/LeverTwo", 0);


    // Wait for both semaphores to be available
    sem_wait(door_sem_1);
    sem_wait(door_sem_2);

    // Get the treasure from the dungeon
    dungeon->spoils[0] = dungeon->treasure[0];

    printf("Rogue: has already '%s'\n", dungeon->spoils);

    while (strlen(dungeon->spoils) <= 4) {
        dungeon->spoils[0] = dungeon->treasure[0];
        dungeon->spoils[1] = dungeon->treasure[1];
        dungeon->spoils[2] = dungeon->treasure[2];
        dungeon->spoils[3] = dungeon->treasure[3]; 
        usleep(10000);
    }
    
    
    dungeon->treasure[4] = '\0';
    dungeon->spoils[4] = '\0';
    // Copy the treasure to the spoils field
    //strcpy(dungeon->spoils, dungeon->treasure);
    

    // Release the semaphores
    sem_post(door_sem_1);
    sem_post(door_sem_2);

    }
}

int main() {

    int shm = shm_open(dungeon_shm_name, O_RDWR, 0);
    // Map the shared memory into the process's address space
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

    struct sigaction sa;
    sa.sa_handler = &signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(DUNGEON_SIGNAL, &sa, NULL);
    sigaction(SEMAPHORE_SIGNAL, &sa, NULL);
    
    sleep(1);
    while(dungeon->running){
        pause();
    }
    
    
    //sleep(SECONDS_TO_ATTACK);
    munmap(dungeon, sizeof(struct Dungeon));
    shm_unlink(dungeon_shm_name);
    return 0;
}