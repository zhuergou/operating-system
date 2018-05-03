/* ctype-and-atoi.c */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main()
{
  char buffer[100];
  sprintf( buffer, "%d56abcdefghijklmnopqrstuvwxyz", 1234 );  /* "1234\0" */
  printf( "buffer is: %s\n", buffer );

*(buffer+17) = '\0';
printf( "\"%s\"\n", buffer + 7 );

  int n = atoi( buffer );    /* also atol() */

  printf( "Converted \"%s\" to %d\n", buffer, n );

  if ( isdigit( buffer[1] ) )
  {
    printf( "'%c' is a digit\n", buffer[1] );
  }

  if ( isalpha( buffer[2] ) )    /* isalnum(), ispunct(), isspace(), etc. */
  {
    printf( "'%c' is an alpha character\n", buffer[2] );
  }

  return EXIT_SUCCESS;
}