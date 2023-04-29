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
    
    //open shared memory
    int fd = shm_open(dungeon_shm_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    ftruncate(fd, sizeof(struct Dungeon));

    struct Dungeon *dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    dungeon->running = true;
    
    //set up semaphores
    sem_t *sem_one = sem_open("/LeverOne", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    sem_t *sem_two = sem_open("/LeverTwo", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);


    // Fork processes for barbarian, wizard, and rogue
    sleep(1);
    pid_t barbarian_pid = fork();
    sleep(1);
    if (barbarian_pid == 0) {
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
        execl("./rogue.o", "./rogue", NULL);
    }

    //run dungeon
    RunDungeon(wizard_pid, rogue_pid, barbarian_pid);

    //close the semaphores
    sem_close(sem_one);
    sem_close(sem_two);

    return 0;
}