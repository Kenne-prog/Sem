#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <ctype.h>
#include <semaphore.h>
#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon* dungeon;

//this will decrypt
char* decrypt(char* spell, int key) {
    char* decrypted = malloc(SPELL_BUFFER_SIZE); 
    for (int i = 0; i < SPELL_BUFFER_SIZE; i++) {
        int chara = (int) spell[i];
        //uppercase
        if (chara >= 65 && chara <= 90) {  
            chara = ((chara - key - 65 + 26) % 26) + 65; 
        }
        //lowercase
        else if (chara >= 97 && chara <= 122) {  
            chara = ((chara - key - 97 + 26) % 26) + 97;  
        }
        decrypted[i] = (char) chara;
    }
    return decrypted;
}

void signal_handler(int signal) {
    //if dungeon signal then decrypt
    if(signal == DUNGEON_SIGNAL){
        //gets the key
        int key = (int) dungeon->barrier.spell[0] % 26;
        //decrypts and copies
        char* decrypted = decrypt(dungeon->barrier.spell + 1, key);
        strcpy(dungeon->wizard.spell, decrypted);  
        free(decrypted);   
    }
    //if semaphore signal holds down lever one
    else if (signal == SEMAPHORE_SIGNAL){
        sem_t *door_sem_1 = sem_open("/LeverOne", 0);
        sem_post(door_sem_1);
        if (strlen(dungeon->spoils) == 4){
            sem_t *door_sem_1 = sem_open("/LeverOne", 0);
        }
    }
}


int main() {
//open shared memory
    int shm_fd = shm_open(dungeon_shm_name, O_RDWR, 0);

    // Map the shared memory into the process's address space
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    //signal hnadler
    struct sigaction sa;
    sa.sa_handler = &signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(DUNGEON_SIGNAL, &sa, NULL);
    sigaction(SEMAPHORE_SIGNAL, &sa, NULL);

    //wait for the signal
    while(dungeon->running){
        pause();
    }
    // unmap shared memory object from process address space
    munmap(dungeon, sizeof(struct Dungeon));
    shm_unlink(dungeon_shm_name);
    return 0;
}