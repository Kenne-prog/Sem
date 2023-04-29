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

struct Dungeon* dungeon;  // initializes the dungeon struct in this class

void rogue_signal_handler(int rec_signals) {
    if (rec_signals == DUNGEON_SIGNAL){
        bool p = false; // sets pick to false
        float max_pick = 100; // max
        float min_pick = 0; // min
        float half = ceil((max_pick + min_pick) / 2); // half
        // while not within the threshold and picked is not the right value it will run
        while (!p && (max_pick - min_pick) > LOCK_THRESHOLD) {
            dungeon->rogue.pick = half;
            usleep(TIME_BETWEEN_ROGUE_TICKS * 2);
            if (dungeon->trap.locked) { // if the trap is locked
                // if rogue's pick needs to go up
                if (dungeon->trap.direction == 'u'){
                    min_pick = half; // sets the min to only account for upper half
                } 
                // if rogue's pick needs to go down
                else if (dungeon->trap.direction == 'd'){
                    max_pick = half; // sets the max to only account for lower half
                }
                half = ceil((max_pick + min_pick) / 2); // gets the new half of current pick
            } 
            else if (dungeon->trap.direction == '-') { // if the pick is within the lock threshold
                    p = true; // sets pick to true
            }
        }
    }
    else if (rec_signals == SEMAPHORE_SIGNAL){

        int treasureCnt = 4; // treasure count
        char nt = '\0'; // null terminator
        sem_t *sem_one = sem_open(dungeon_lever_one, 0); // opens the semaphore to lever one
        sem_t *sem_two = sem_open(dungeon_lever_two, 0); // opens the semaphore to lever two

        sem_wait(sem_one); // decrements the semaphore of lever one
        sem_wait(sem_two); // decrements the semaphore of lever two

        while (strlen(dungeon->spoils) <= 4) { // while there is still empty spoils it will run
            // copies all of the treasures into the spoils
            dungeon->spoils[0] = dungeon->treasure[0];
            dungeon->spoils[1] = dungeon->treasure[1];
            dungeon->spoils[2] = dungeon->treasure[2];
            dungeon->spoils[3] = dungeon->treasure[3]; 
            usleep(TIME_BETWEEN_ROGUE_TICKS);
        }
        dungeon->spoils[4] = nt; // replaces last element in spoils with null terminator
        dungeon->treasure[4] = nt; // replaces last element in treasure with null terminator
        
        sem_post(sem_one); // increments the semaphore of lever one
        sem_post(sem_two); // increments the semaphore of lever one
    }
}

int main(int argc, char *argv[]) {

    struct sigaction act; // creates sigaction struct
    act.sa_handler = &rogue_signal_handler; // calls the signal handler for wizard 
    sigaction(DUNGEON_SIGNAL, &act, NULL); // changes the action of the process to dungeon signal
    sigaction(SEMAPHORE_SIGNAL, &act, NULL); // changes the action of the process to semaphore signal

    int fd = shm_open(dungeon_shm_name, O_RDWR, 0); // opens the shared memory segment
    // maps the shared memeory
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    sleep(1); // sleeps to wait for signals
    while(dungeon->running){ // pauses dungeon
        pause();
    }

    munmap(dungeon, sizeof(struct Dungeon)); // unmaps the shared memory
    shm_unlink(dungeon_shm_name); // removes the shared memory

    return 0;
}