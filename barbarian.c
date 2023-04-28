#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <semaphore.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>

struct Dungeon* dungeon;
void signal_handler(int signal) {
    if (signal == DUNGEON_SIGNAL){
        dungeon->barbarian.attack = dungeon-> enemy.health;
    }
    else if (signal == SEMAPHORE_SIGNAL){
        sem_t *door_sem_2 = sem_open("/LeverTwo", 0);
        sem_post(door_sem_2);
        if (strlen(dungeon->spoils) == 4){
            sem_t *door_sem_2 = sem_open("/LeverTwo", 0);
        }
    }
}


int main() {

    int shm_fd = shm_open(dungeon_shm_name, O_RDWR, 0);

    // Map the shared memory into the process's address space
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    struct sigaction sa;
    sa.sa_handler = &signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(DUNGEON_SIGNAL, &sa, NULL);
    sigaction(SEMAPHORE_SIGNAL, &sa, NULL);

    while(dungeon->running){
        pause();
    }
    
    //sleep(SECONDS_TO_ATTACK);
    munmap(dungeon, sizeof(struct Dungeon));
    shm_unlink(dungeon_shm_name);
    return 0;
}