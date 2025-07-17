// sbrk() is a UNIX-specific system call used for directly adjusting the process’s heap
#include <unistd.h>

#define mem_alloc(x) \
    sbrk(x);         \
    _allocated_mem += x;

#define free_mem()         \
    sbrk(-_allocated_mem); \
    _allocated_mem = 0;

unsigned int _allocated_mem;
