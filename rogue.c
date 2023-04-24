#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon* dungeon;

void barbarian_signal_handler(int signal) {
    if (signal == DUNGEON_SIGNAL) {
        printf("RogueIKE");
    }
}

int main() {
    
    
    struct sigaction sa;
    sa.sa_handler = barbarian_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(DUNGEON_SIGNAL, &sa, NULL);
    
    int fd = shm_open(dungeon_shm_name, O_RDWR, 0666);
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    kill(0, DUNGEON_SIGNAL);

    while(dungeon->running){
        pause();
    }
    // Send SIG_BARBARIAN_ATTACK signal to self (pid = 0)
    

    
    if (munmap(dungeon, sizeof(struct Dungeon)) == -1) {
        perror("munmap");
    }
    
    
    return 0;
}