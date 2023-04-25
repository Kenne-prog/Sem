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

void decrypt(char message[], int key)
{
    int i;
    char ch;

    for(i = 0; message[i] != '\0'; i++)
    {
        ch = message[i];

        if(ch >= 'A' && ch <= 'Z')
        {
            ch = ((ch - 'A') - key + 26) % 26 + 'A';
        }
        else if(ch >= 'a' && ch <= 'z')
        {
            ch = ((ch - 'a') - key + 26) % 26 + 'a';
        }

        message[i] = ch;
    }
}

// Signal handler function for SIGUSR1
void signal_handler(int signal) {
    char code[] = dungeon->barrier.spell;
    char key = code[0];
    printf("%c", key);
    decrypt(dungeon->wizard.spell, key);
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