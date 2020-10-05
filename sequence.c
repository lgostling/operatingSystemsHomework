#include "common.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>

sem_t *lock;

// Print out an error message and exit.
static void fail( char const *message ) {
    #ifndef UNSAFE
    sem_post(lock);
    #endif
    fprintf( stderr, "%s\n", message );
    exit( EXIT_FAILURE );
}

// Print out usage message and exit.
static void usage() {
    #ifndef UNSAFE
    sem_post(lock);
    #endif
    fprintf( stderr, "usage:\tsequence report\n\tsequence swap <idx1> <idx2>\n\tsequence inc <idx>\n\tsequence dec <idx>\n");
    exit( EXIT_FAILURE );
}

int toInt(char* c) {
    int l = strlen(c);
    if(c[0] != '-' && (c[0] < '0' || c[0] > '9')) {
        fail("error");
    }
    for(int i = 1; i < l; i++) {
        if(c[0] < '0' || c[0] > '9') {
            fail("error");
        }
    }
    return atoi(c);
}

int main( int argc, char *argv[] ) {
    lock = sem_open("/lpgostli-sequence-lock", O_EXCL);
    #ifndef UNSAFE
    sem_wait(lock);
    #endif

    char *path = "C:/Users/lukegostling/Desktop/school/csc246/homework/2";
    key_t key = ftok(path, 'S');
    int id = shmget(key, sizeof(Sequence), 0666 | IPC_CREAT);
    struct Sequence *ptr = (struct Sequence*) shmat(id, 0, 0);

    if(strcmp(argv[1], "report") == 0) {
        if(argc != 2) { usage(); }
        for(int i = 0; i < ptr->numValues; i++) {
            printf("%d ", ptr->values[i]);
        }
        #ifndef UNSAFE
        sem_post(lock);
        #endif
        printf("\n");
        return EXIT_SUCCESS;
    } else if(strcmp(argv[1], "test") == 0) {
        int n = toInt(argv[2]);
        for(int i = 0; i < n; i++) {
            int x = ptr->values[0];
            ptr->values[0] = ptr->values[1];
            ptr->values[1] = x;
        }
        #ifndef UNSAFE
        sem_post(lock);
        #endif
        exit(EXIT_SUCCESS);
    } else if(strcmp(argv[1], "inc") == 0) {
        if(argc != 3) { usage(); }
        int i = toInt(argv[2]);
        if(i < 0 || i >= ptr->numValues) {
            fail("error");
        }
        ptr->values[i]++;
    } else if(strcmp(argv[1], "dec") == 0) {
        if(argc != 3) { usage(); }
        int i = toInt(argv[2]);
        if(i < 0 || i >= ptr->numValues) {
            fail("error");
        }
        ptr->values[i]--;
    } else if(strcmp(argv[1], "swap") == 0) {
        if(argc != 4) { usage(); }
        int a = toInt(argv[2]);
        int b = toInt(argv[3]);
        if(a < 0 || a >= ptr->numValues) {
            fail("error");
        }
        if(b < 0 || b >= ptr->numValues) {
            fail("error");
        }
        int x = ptr->values[a];
        ptr->values[a] = ptr->values[b];
        ptr->values[b] = x;
    } else {
        usage();
    }
    #ifndef UNSAFE
    sem_post(lock);
    #endif
    printf("success\n");
    return EXIT_SUCCESS;
}
