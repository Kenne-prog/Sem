#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>

struct Dungeon* dungeon;
void signal_handler(int signum) {
    dungeon->barbarian.attack = dungeon-> enemy.health;
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