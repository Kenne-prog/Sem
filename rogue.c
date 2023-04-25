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
    float min = 0;
    float max = 100;
    float mid = ceil((min + max) / 2);
    bool picked = false;

    while (!picked && (max - min) > LOCK_THRESHOLD) {
        dungeon->rogue.pick = mid;
        usleep(100000);

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

void sem_handler(int signal) {
    printf("SEMAPHIRE SIGNAMAL");
    sem_t *door_sem_1 = sem_open("/LeverOne", 0);
    sem_t *door_sem_2 = sem_open("/LeverTwo", 0);
    printf("Rogue: Found word '%s'\n", dungeon->treasure);
    // Wait for both semaphores to be available
    sem_wait(door_sem_1);
    sem_wait(door_sem_2);

    // Get the treasure from the dungeon
    for (int i = 0; i < 4; i++) {
        dungeon->spoils[i] = dungeon->treasure[i];
        printf("Rogue: Found character '%c'\n", dungeon->treasure[i]);
        usleep(100000);
    }

    // Copy the treasure to the spoils field
    strncpy(dungeon->treasure, dungeon->spoils, 4);
    printf("Rogue: Copied treasure to spoils field: %s\n", dungeon->spoils);

// Release the semaphores
    sem_post(door_sem_1);
    sem_post(door_sem_2);

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
    
    struct sigaction sem_signal;
    sem_signal.sa_handler = &sem_handler;
    sigemptyset(&sem_signal.sa_mask);
    sem_signal.sa_flags = 0;
    sigaction(SEMAPHORE_SIGNAL, &sem_signal, NULL);

    while(dungeon->running){
        pause();
    }
    
    
    //sleep(SECONDS_TO_ATTACK);
    munmap(dungeon, sizeof(struct Dungeon));
    shm_unlink(dungeon_shm_name);
    return 0;
}