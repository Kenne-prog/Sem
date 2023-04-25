#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <sys/ipc.h>    
#include <sys/shm.h> 
#include <string.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"


#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE);} while (0)
struct Dungeon* dungeon;
int main(int argc, char *argv[]) {
    
    int shm_fd = shm_open(dungeon_shm_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    ftruncate(shm_fd, sizeof(struct Dungeon);

    struct Dungeon *dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);


    // Fork processes for barbarian, wizard, and rogue
    dungeon->running = true;
    
    sleep(1);
    pid_t barbarian_pid = fork();
    sleep(1);
    if (barbarian_pid == 0) {
        // Child process for Barbarian
        execl("./barbarian.o", "./barbarian", NULL);
    }

    sleep(1);
    pid_t wizard_pid = fork();
    sleep(1);
    if (wizard_pid == 0) {
        execl("./wizard.o", "./wizard", NULL);
    }

    sleep(1);
    pid_t rogue_pid = fork();
    sleep(1);
    if (rogue_pid == 0) {
        // Child process for Rogue
        execl("./rogue.o", "./rogue", NULL);
    }
    
    
    // Wait for processes to complete

    RunDungeon(wizard_pid, rogue_pid, barbarian_pid);

    if (munmap(dungeon, sizeof(struct Dungeon)) == -1) {
        perror("Error unmapping shared memory");
        exit(EXIT_FAILURE);
    }
    if (shm_unlink(dungeon_shm_name) == -1) {
        perror("Error unlinking shared memory");
        exit(EXIT_FAILURE);
    }

    return 0;
}