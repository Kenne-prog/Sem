#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"
struct Dungeon* dungeon;


// Signal handler function for SIGUSR1
void signal_handler(int signal) {
    // do nothing
}

int main() {
    // Initialize shared memory
    int shm_fd = shm_open(dungeon_shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error creating shared memory");
        return 1;
    }
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (dungeon == MAP_FAILED) {
        perror("Error mapping shared memory");
        return 1;
    }

    // Set up signal handler
    struct sigaction sa;
    sa.sa_handler = &signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(DUNGEON_SIGNAL, &sa, NULL);
    while(dungeon->running){
        pause();
    }
    

    return 0;
}