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


char* caesar_cipher(char* spell, int shift) {
    //int len = strlen(spell);
    char* encrypted = malloc(SPELL_BUFFER_SIZE); // allocate memory for the encrypted string
    for (int i = 0; i < SPELL_BUFFER_SIZE; i++) {
        int ch = (int) spell[i];
        
        if (ch >= 65 && ch <= 90) {   // uppercase letters
            ch = ((ch - shift - 65 + 26) % 26) + 65;   // shift and wrap around
        }
        else if (ch >= 97 && ch <= 122) {   // lowercase letters
            ch = ((ch - shift - 97 + 26) % 26) + 97;   // shift and wrap around
        }
        encrypted[i] = (char) ch;
    }
    return encrypted;
}

void signal_handler(int signal) {
    if(signal == DUNGEON_SIGNAL){
        // set global variable to signal received
        int shift = (int) dungeon->barrier.spell[0] % 26;   // get the first character as the shift value
        //sleep(SECONDS_TO_GUESS_BARRIER);
        char* encrypted = caesar_cipher(dungeon->barrier.spell + 1, shift);   // pass the rest of the string to caesar_cipher
        strcpy(dungeon->wizard.spell, encrypted);   // copy the encrypted string back to the struct
        free(encrypted);   // free the memory allocated for the encrypted string
    }
    else if (signal == SEMAPHORE_SIGNAL){
        sem_t *door_sem_1 = sem_open("/LeverOne", O_RDWR);
        sem_t *door_sem_2 = sem_open("/LeverTwo", O_RDWR);


        printf("Wizard: Entering the treasure room\n");
        sem_post(door_sem_2);
    }
    
}


int main() {

    // open shared memory object
    int shm_fd = shm_open(dungeon_shm_name, O_RDWR, 0);

    // map shared memory object to process address space
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

    // unmap shared memory object from process address space
    munmap(dungeon, sizeof(struct Dungeon));
    shm_unlink(dungeon_shm_name);
    return 0;
}