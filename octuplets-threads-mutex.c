/* octuplets-threads-mutex.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CHILDREN 8   /* based on a true story.... */

/* function executed by each thread */
void * whattodo( void * arg );


/* global mutex variable to synchronize the child threads */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int main()
{
  pthread_t tid[CHILDREN];   /* keep track of the thread IDs */
  int i, rc;

  /* create all the child threads */
  for ( i = 0 ; i < CHILDREN ; i++ )
  {
    int * t = malloc( sizeof( int ) );
    *t = 2 + i * 2;   /* 2, 4, 6, 8, ... */

    printf( "MAIN: Creating a thread to sleep for %d seconds\n", *t );

    rc = pthread_create( &tid[i], NULL, whattodo, t );

    if ( rc != 0 )
    {
      fprintf( stderr, "MAIN: Could not create thread (%d)\n", rc );
      return EXIT_FAILURE;
    }
  }

  /* wait for child threads to terminate */
  for ( i = 0 ; i < CHILDREN ; i++ )
  {
    unsigned int * x;
    rc = pthread_join( tid[i], (void **)&x );   /* BLOCKING CALL */

    if ( rc != 0 )
    {
      fprintf( stderr, "MAIN: Could not join thread (%d)\n", rc );
    }
    else
    {
      printf( "MAIN: Joined child thread %u, which returned %u\n",
              (unsigned int)tid[i], *x );
      free( x );
    }
  }

  printf( "MAIN: All threads terminated\n" );

  return EXIT_SUCCESS;
}


void critical_section( int t )
{
  printf( "THREAD %u: I'm going to nap for %d seconds\n",
          (unsigned int)pthread_self(), t );
  sleep( t );
  printf( "THREAD %u: I'm awake\n", (unsigned int)pthread_self() );
}


void * whattodo( void * arg )
{
  int t = *(int *)arg;
  free( arg );

  pthread_mutex_lock( &mutex );
    critical_section( t );
  pthread_mutex_unlock( &mutex );

  /* dynamically allocate memory to hold the thread's return value */
  unsigned int * x = malloc( sizeof( unsigned int ) );
  *x = pthread_self();
  pthread_exit( x );
}

