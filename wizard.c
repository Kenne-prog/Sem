#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <semaphore.h>
#include <string.h>

#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon* dungeon; // initializes the dungeon struct in this class

char* caesar_cypher(char* encrypted, int shift) { // caesar cypher shift function
    char* answer = malloc(SPELL_BUFFER_SIZE); // allocates the memory for the answer 
    shift = shift % 26; // modulus the the value of the first character by the len of the alphabet

    for (int i = 0; i < SPELL_BUFFER_SIZE; i++) { // runs through each character in the encrypted phrase
        int letter = (int) encrypted[i]; // gets each character as it runs through loop
        // if statements checks if the character is a letter in the alphabet and for caps/lower cases (skips spaces and punctuation)
        if (letter >= 65 && letter <= 90) { // checks if the letter is capitalized
            // left shift from the letter to decrypt, subtract by 'A', add len of alphabet, mod 26 and add 'A' for roll
            letter = ((letter - shift - 65 + 26) % 26) + 65;
        }
        else if (letter >= 97 && letter <= 122) { // checks if the letter is a lower cased
            // left shift from the letter to decrypt, subtract by 'a', add len of alphabet, mod 26 and add 'a' for roll 
            letter = ((letter - shift - 97 + 26) % 26) + 97;
        }
        answer[i] = (char) letter; // places decrpyted letter into the answer  
    }
    return answer;
}

void wiz_signal_handler(int signals) { // handler to access shared memory upon receiving signal
    if (signals == DUNGEON_SIGNAL){
        // calls caesar cypher function with encrypted spell and first letter as shift
        char* decrypted = caesar_cypher(dungeon->barrier.spell + 1, dungeon->barrier.spell[0]);
        strcpy(dungeon->wizard.spell, decrypted); // copies in the decrpyted message into the wizard's spell field
    }
    else if (signals == SEMAPHORE_SIGNAL){
        sem_t *sem_two = sem_open(dungeon_lever_two, 0);
        sem_post(sem_two);
        if (strlen(dungeon->spoils) == 4){
            sem_t *sem_two = sem_open(dungeon_lever_two, 0);
        }
    }
}

int main() {
    int fd = shm_open(dungeon_shm_name, O_RDWR, 0); // opens the shared memory segment
    // maps the shared memeory
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    struct sigaction act; // creates sigaction struct
    act.sa_handler = &wiz_signal_handler; // calls the signal handler for wizard 
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(DUNGEON_SIGNAL, &act, NULL); // changes the action of the process to dungeon signal
    sigaction(SEMAPHORE_SIGNAL, &act, NULL);

    while(dungeon->running){ // pauses dungeon
        pause();
    }
    
    munmap(dungeon, sizeof(struct Dungeon)); // unmaps the shared memory
    shm_unlink(dungeon_shm_name); // removes the shared memory
    
    return 0;
}