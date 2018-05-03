/* shared.h */

#define BUFFER_SIZE 8

/* This constant defines the shared memory segment such that
   multiple processes can attach to this segment */
#define SHM_SHARED_KEY 6767

struct shared_data
{
  int buffer[BUFFER_SIZE];  /* circular array */
  int in;                   /* index of next available producer array slot */
  int out;                  /* index of next array slot to consume from */
  int count;                /* number of elements in the buffer */
};