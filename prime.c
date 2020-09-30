#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( EXIT_FAILURE );
}

// Print out a usage message, then exit.
static void usage() {
  printf( "usage: prime <workers>\n" );
  printf( "       prime <workers> report\n" );
  exit( 1 );
}

// True if we're supposed to report what we find.
bool report = false;

// Total number of prime numbers we've found.
int total = 0;

// Fixed-sized array for holding the input values.
#define MAX_VALUES 500000
int vList[ MAX_VALUES ];

// Number of values on the list.
int vCount = 0;

// number of values that have been assigned to workers
int assignedCount = 0;

// flag for when all work has been done
bool done = false;

// semaphores for incrementing total and assigning work
sem_t countLock;
sem_t workLock;

int getWork() {
  while(true) {
    if(done && assignedCount == vCount) {
      return -1;
    }
    sem_wait(&workLock);
    if(assignedCount < vCount) {
      int v = assignedCount;
      assignedCount += 1;
      sem_post(&workLock);
      return vList[v];
    }
    sem_post(&workLock);
  }
}

// Read the list of input values.
void readList() {

  // Keep reading as many input values as we can.
  int v;
  while ( scanf( "%d", &v ) == 1 ) {
    // Make sure we have enough room, then store it on the list.
    if ( vCount > MAX_VALUES )
      fail( "Too many input values" );
    
    // Store the latest value
    vList[ vCount ] = v;
    vCount += 1;
  }
}

/** Start routine for each worker. */
void *workerRoutine( void *arg ) {
  int p = getWork();
  while(p != -1) {
    // check value for prime
    bool prime = true;
    if(p % 2 == 0) {
      prime = false;
    } else {
      for(int i = 3; i * i <= p; i += 2) {
        if(p % i == 0) {
          prime = false;
          break;
        }
      }
    }
    
    if(prime) {
      // increment count
      sem_wait(&countLock);
      total += 1;
      sem_post(&countLock);
      // report as nessisary
      if(report) {
        printf("%d\n", p);
      }
    }
    p = getWork();
  }

  return NULL;
}

int main( int argc, char *argv[] ) {
  int workers = 4;
  
  // Parse command-line arguments.
  if ( argc < 2 || argc > 3 )
    usage();
  
  if ( sscanf( argv[ 1 ], "%d", &workers ) != 1 ||
       workers < 1 )
    usage();

  // If there's a second argument, it better be "report"
  if ( argc == 3 ) {
    if ( strcmp( argv[ 2 ], "report" ) != 0 )
      usage();
    report = true;
  }

  sem_init(&workLock, 0, 1);
  sem_init(&countLock, 0, 1);

  // Make each of the workers.
  pthread_t worker[ workers ];
  for ( int i = 0; i < workers; i++ ) {
    pthread_t w;
    pthread_create(&w, NULL, workerRoutine, NULL);
    worker[i] = w;
  }

  // Then, start getting work for them to do.
  readList();
  done = true;

  // Wait until all the workers finish.
  for ( int i = 0; i < workers; i++ ) {
    pthread_join(worker[i], NULL);
  }

  // Report the total and release the semaphores.
  printf( "Prime count: %d\n", total );
  sem_destroy(&countLock);
  sem_destroy(&workLock);
  
  return EXIT_SUCCESS;
}
