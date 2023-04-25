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
#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon* dungeon;

float min = 0;
float max = MAX_PICK_ANGLE;

// Binary search function to pick the lock
void signal_handler(int signal) {
    dungeon->rogue.pick = 0.0;
    float mid = ceil((min + max) / 2);
    bool solved = false;

    while (!solved) {
        
        dungeon->rogue.pick = mid;
        usleep(TIME_BETWEEN_ROGUE_TICKS*10);
        
        if (dungeon->trap.locked) {
            if (dungeon->trap.direction == 'u') {
                min = mid;
            } else {
                max = mid;
            }
            mid = ceil((min + max) / 2);
        
            if (mid == dungeon->rogue.pick) {
                float min = 0;
                float max = MAX_PICK_ANGLE;
            }
        
        } else if (dungeon->trap.direction == '-') {
            solved = true;
        }
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
    
    while(dungeon->running){
        pause();
    }
    float min = 0;
    float max = MAX_PICK_ANGLE;
    //sleep(SECONDS_TO_ATTACK);
    munmap(dungeon, sizeof(struct Dungeon));
    shm_unlink(dungeon_shm_name);
    return 0;
}