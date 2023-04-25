#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
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


struct Dungeon* dungeon;
int main(int argc, char *argv[]) {
    
    int shm = shm_open(dungeon_shm_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    ftruncate(shm, sizeof(struct Dungeon));

    struct Dungeon *dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

    // Fork processes for barbarian, wizard, and rogue
    dungeon->running = true;
    
    sem_t *door_sem_1 = sem_open("/LeverOne", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    sem_t *door_sem_2 = sem_open("/LeverTwo", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);

    sleep(1);
    pid_t barbarian_pid = fork();
    sleep(1);
    if (barbarian_pid == 0) {
        execl("./barbarian.o", "./barbarian", "/LeverOne","/LeverTwo", NULL);
    }

    sleep(1);
    pid_t wizard_pid = fork();
    sleep(1);
    if (wizard_pid == 0) {
        execl("./wizard.o", "./wizard", "/LeverOne","/LeverTwo", NULL);
    }

    sleep(1);
    pid_t rogue_pid = fork();
    sleep(1);
    if (rogue_pid == 0) {
        execl("./rogue.o", "./rogue","/LeverOne","/LeverTwo", NULL);
    }
    
    // Wait for processes to complete
    sem_close(door_sem_1);
    sem_close(door_sem_2);

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