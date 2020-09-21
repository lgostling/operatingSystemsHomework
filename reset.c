// Author : Luke Gostling + teaching staff 

#include "common.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>


// Print out an error message and exit.
static void fail( char const *message ) {
    fprintf( stderr, "%s\n", message );
    exit( EXIT_FAILURE );
}

void numberString(int num, int *values, char* str) {
    int index = 0;
    for(int i = 0; i < num; i++) {
        index += sprintf(&str[index], "%d ", values[i]);
    }
}

int main( int argc, char *argv[] ) {

    bool invalid = false;
    // error check inputs
    if(argc > 11 || argc < 1) {
        invalid = true;
    }

    if(!invalid) { 
        for(int i = 0; i < argc - 1; i++) {
            for(int c = 0; c < strlen( argv[i + 1] ); c++) {
                if((argv[i + 1][c] < '0' || argv[i + 1][c] > '9') && argv[i + 1][c] != '-') {
                    invalid = true;
                }
            }
        }
    }

    if(invalid) {
        fail("Invalid list of values");
    }

    char *path = "C:/Users/lukegostling/Desktop/school/csc246/homework/2";
    key_t key = ftok(path, 'S');
    int id = shmget(key, sizeof(Sequence), 0666 | IPC_CREAT);
    struct Sequence *ptr = (struct Sequence*) shmat(id, 0, 0);

    ptr->numValues = argc - 1;
    for(int i = 0; i < argc - 1; i++) {
        ptr->values[i] = atoi(argv[i + 1]);
    }

    return EXIT_SUCCESS;
}