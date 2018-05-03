/* octuplets-threads-broken.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CHILDREN 8   /* based on a true story.... */

/* function executed by each thread */
void * whattodo( void * arg );

int main()
{
  pthread_t tid[CHILDREN];   /* keep track of the thread IDs */
  int i, rc;

  int t;  /* STATICALLY allocated memory (4 bytes) on the runtime stack */
    /* whoops....all threads share the same runtime stack (and heap) */

  /* create all the child threads */
  for ( i = 0 ; i < CHILDREN ; i++ )
  {
    t = 2 + i * 2;   /* 2, 4, 6, 8, ... */

    printf( "MAIN: Creating a thread to sleep for %d seconds\n", t );

    rc = pthread_create( &tid[i], NULL, whattodo, &t );
                           /*                    ^^^^
                                      all threads get the same
                                       statically allocated memory address */

    if ( rc != 0 )
    {
      fprintf( stderr, "MAIN: Could not create thread (%d)\n", rc );
      return EXIT_FAILURE;
    }
  }

  /* wait for child threads to terminate */
  for ( i = 0 ; i < CHILDREN ; i++ )
  {
    rc = pthread_join( tid[i], NULL );   /* BLOCKING CALL */

    if ( rc != 0 )
    {
      fprintf( stderr, "MAIN: Could not join thread (%d)\n", rc );
    }
    else
    {
      printf( "MAIN: Joined child thread %u\n", (unsigned int)tid[i] );
    }
  }

  printf( "MAIN: All threads terminated\n" );

  return EXIT_SUCCESS;
}

void * whattodo( void * arg )
{
  int t = *(int *)arg;
  printf( "THREAD %u: I'm going to nap for %d seconds\n",
          (unsigned int)pthread_self(), t );
  sleep( t );
  printf( "THREAD %u: I'm awake\n", (unsigned int)pthread_self() );

  return NULL;
}