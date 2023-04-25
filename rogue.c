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


// Binary search function to pick the lock
void signal_handler(int signal) {
    float min = 0;
    float max = MAX_PICK_ANGLE;
    float mid = ceil((min + max) / 2);
    bool success = false;

    while (!success && (max - min) > LOCK_THRESHOLD) {
        dungeon->rogue.pick = mid;
        usleep(TIME_BETWEEN_ROGUE_TICKS);

        if (dungeon->trap.locked) {
            if (dungeon->trap.direction == 'u') {
                max = mid;
            } else {
                min = mid;
            }
            mid = (min + max) / 2;
        } else if (dungeon->trap.direction == '-') {
            success = true;
        }
    }

    return success;
}

int main() {

    int shm_fd = shm_open(dungeon_shm_name, O_RDWR, 0);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }
    // Map the shared memory into the process's address space
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (dungeon == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    struct sigaction sa;
    sa.sa_handler = &signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(DUNGEON_SIGNAL, &sa, NULL);
    
    while(dungeon->running){
        pause();
    }
    
    //sleep(SECONDS_TO_ATTACK);
    munmap(dungeon, sizeof(struct Dungeon));
    shm_unlink(dungeon_shm_name);
    return 0;
}