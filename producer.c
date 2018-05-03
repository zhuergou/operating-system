/* producer.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#include "shared.h"

int shmid;
struct shared_data * shm;

void clean_exit( int n )
{
  printf( "exiting....\n" );

  /* detach from the shared memory segment */
  int rc = shmdt( shm );

  if ( rc == -1 )
  {
    perror( "shmdt() failed" );
    exit( EXIT_FAILURE );
  }

  /* remove the shared memory segment */
  /* Notes: (1) if this is not called, the shared memory segment
      will stick around "forever" -- until this is eventually called
       or a system reboot occurs
     (2) if this is called, the removal will not occur until all
          all attached processes have detached via shmdt() or
           terminated */
  if ( shmctl( shmid, IPC_RMID, 0 ) == -1 )
  {
    perror( "shmctl() failed" );
    exit( EXIT_FAILURE );
  }

  exit( EXIT_SUCCESS );
}

int main()
{
  signal( SIGINT, clean_exit );

  key_t key = SHM_SHARED_KEY;
  shmid = shmget( key, sizeof( struct shared_data ),
                  IPC_CREAT | /*IPC_EXCL |*/ 0660 );

  printf( "shmget() returned %d\n", shmid );

  if ( shmid == -1 )
  {
    perror( "shmget() failed" );
    return EXIT_FAILURE;
  }


  shm = shmat( shmid, NULL, 0 );
  if ( shm == (struct shared_data *)-1 )
  {
    perror( "shmat() failed" );
    return EXIT_FAILURE;
  }


  int item = 1;
  shm->in = shm->out = shm->count = 0;

  while ( item < 200 )
  {
    while ( shm->count == BUFFER_SIZE )
    {
      /* busy wait */
      printf( "PRODUCER: buffer full......\n" );
/*      sleep( 1 ); */
    }

    printf( "PRODUCER: producing item %d\n", item );
    shm->buffer[ shm->in ] = item;
    item++;
    shm->in = ( shm->in + 1 ) % BUFFER_SIZE;    /* circular array */
    shm->count++;

/*    sleep( 1 ); */
  }

  clean_exit( 0 );

  return EXIT_SUCCESS;
}