#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <semaphore.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>

struct Dungeon* dungeon;
void signal_handler(int signum) {
    dungeon->barbarian.attack = dungeon-> enemy.health;
}

void sem_handler(int signum) {
    sem_t *door_sem_1 = sem_open("/LeverOne", 0);
    sem_t *door_sem_2 = sem_open("/LeverTwo", 0);

    // Wait for both semaphores to be available
    sem_wait(door_sem_1);
    sem_wait(door_sem_2);

    // Attack the enemy

    // Release the semaphores
    sem_post(door_sem_1);
    sem_post(door_sem_2);
}

int main() {

    int shm_fd = shm_open(dungeon_shm_name, O_RDWR, 0);

    // Map the shared memory into the process's address space
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    struct sigaction signal;
    signal.sa_handler = &signal_handler;
    sigemptyset(&signal.sa_mask);
    signal.sa_flags = 0;
    sigaction(DUNGEON_SIGNAL, &signal, NULL);
    
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