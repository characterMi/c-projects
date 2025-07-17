#include <windows.h>
#include <stdio.h>

void *_base_ptr = NULL;
unsigned int _allocated_mem = 0;

#define mem_alloc(x)                                                          \
    do                                                                        \
    {                                                                         \
        if (!_base_ptr)                                                       \
        {                                                                     \
            _base_ptr = VirtualAlloc(NULL, 1 << 20, MEM_RESERVE | MEM_COMMIT, \
                                     PAGE_READWRITE);                         \
            if (!_base_ptr)                                                   \
            {                                                                 \
                fprintf(stderr, "Initial allocation failed\n");               \
                exit(EXIT_FAILURE);                                           \
            }                                                                 \
        }                                                                     \
        _allocated_mem += x;                                                  \
    } while (0)

#define free_mem()                                  \
    do                                              \
    {                                               \
        if (_base_ptr)                              \
        {                                           \
            VirtualFree(_base_ptr, 0, MEM_RELEASE); \
            _base_ptr = NULL;                       \
            _allocated_mem = 0;                     \
        }                                           \
    } while (0)
