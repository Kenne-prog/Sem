#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>    
#include <sys/shm.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <semaphore.h>

#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon* dungeon;
sem_t *sem_one;
sem_t *sem_two;

int main(int argc, char *argv[]) {
    int fd = shm_open(dungeon_shm_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); // creates shared memory
  
    ftruncate(fd, sizeof(struct Dungeon)); // sets the size of the memory to Dungeon struct
    // maps the shared memory 
    struct Dungeon *dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    sem_one = sem_open(dungeon_lever_one, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    sem_two = sem_open(dungeon_lever_two, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    //sleep(1);
    pid_t barbarian_pid = fork(); // creates barbarian process
    sleep(SECONDS_TO_ATTACK); // sleep for seconds to attack (barbarian)
    if (barbarian_pid == -1) { // checks for errors creating barbarian process
        perror("Error forking Barbarian process");
    } else if (barbarian_pid == 0) { // if fork success, execs barbarian process
        execl("./barbarian.o", "./barbarian", NULL);
    }
    //sleep(1); 
    pid_t wizard_pid = fork(); // creates wizard process
    sleep(SECONDS_TO_GUESS_BARRIER); // sleep for seconds to guess (wizard)
    if (wizard_pid == -1) { // checks for errors creating wizard process
        perror("Error forking Wizard process");
    } else if (wizard_pid == 0) { // if fork success, execs wizard process
        execl("./wizard.o", "./wizard", NULL);

    }
    //sleep(1); 
    pid_t rogue_pid = fork(); // creates rogue process
    sleep(SECONDS_TO_PICK); // sleep for seconds to pick (rogue)
    if (rogue_pid == -1) { // checks for errors creating rogue process
        perror("Error creating process for rogue");
    } else if (rogue_pid == 0) { // if fork success, execs rogue process
        execl("./rogue.o", "./rogue", NULL);
    }
    

    RunDungeon(wizard_pid, rogue_pid, barbarian_pid); // calls RunDungeon with character PIDS

    sem_close(sem_one);
    sem_close(sem_two);

    munmap(dungeon, sizeof(struct Dungeon)); // unmaps the shared memory
    shm_unlink(dungeon_shm_name); // removes the shared memory
        
    return 0;
}