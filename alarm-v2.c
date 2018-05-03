/* alarm-v2.c */

/* implementation using multiple processes */

/* drawbacks: (1) need to handle zombie/defunct processes
              (2) inefficient in terms of memory -- i.e., overhead of
                   a large number of processes
              (3) upper limit on the number of alarms that can be set
                   (based on upper limit of number of processes)  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE 80

int main()
{
  char line[MAX_LINE];

  int seconds;
  char msg[MAX_LINE];

  while ( 1 )
  {
    printf( "Set alarm (<sec> <msg>): " );

    if ( fgets( line, MAX_LINE, stdin ) == NULL )
    {
      return EXIT_FAILURE;
    }

    if ( strlen( line ) <= 1 ) continue;   /* skip blank lines */

    if ( sscanf( line, "%d %[^\n]", &seconds, msg ) < 2 || seconds < 0 )
    {
      fprintf( stderr, "ERROR: invalid alarm request\n" );
    }
    else
    {
      /* create a child process to create (and wait) for the alarm */
      pid_t pid = fork();

      if ( pid < 0 )
      {
        perror( "fork() failed" );
        return EXIT_FAILURE;
      }
      else if ( pid == 0 )   /* CHILD */
      {
        printf( "Alarm set for %d seconds\n", seconds );
        sleep( seconds );
        printf( "ALARM (%d): %s\n", seconds, msg );
        exit( EXIT_SUCCESS ); /* exit the CHILD process */
      }
      else /* PARENT */
      {
        /* TO DO: add waitpid() with the WNOHANG option to kill zombies */

        usleep( 10000 );
      }
    }
  }

  return EXIT_SUCCESS;
}
