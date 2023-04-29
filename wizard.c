#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <ctype.h>
#include <semaphore.h>

#include "dungeon_info.h"
#include "dungeon_settings.h"

struct Dungeon* dungeon;

//this will decrypt
char* caesar_cypher(char* encrypted, int shift) {
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

void signal_handler(int signal) {
    //if dungeon signal then decrypt
    if(signal == DUNGEON_SIGNAL){
        // calls caesar cypher function with encrypted spell and first letter as shift
        char* decrypted = caesar_cypher(dungeon->barrier.spell + 1, dungeon->barrier.spell[0]);
        strcpy(dungeon->wizard.spell, decrypted); // copies in the decrpyted message into the wizard's spell field
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
    int fd = shm_open(dungeon_shm_name, O_RDWR, 0);

    // Map the shared memory into the process's address space
    dungeon = mmap(NULL, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    //signal hnadler
    struct sigaction act;
    act.sa_handler = &signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(DUNGEON_SIGNAL, &act, NULL);
    sigaction(SEMAPHORE_SIGNAL, &act, NULL);

    //wait for the signal
    while(dungeon->running){
        pause();
    }

    return 0;
}