
#include <stdlib.h>
#include <stdio.h>

#include "platform.h"
#include "id_allocator.h"

MAIN {
    counter ids[10];

    struct id_allocator *allocator = new_id_allocator();

    ids[0] = id_allocator_acquire(allocator);
    ids[1] = id_allocator_acquire(allocator);
    ids[2] = id_allocator_acquire(allocator);
    id_allocator_release(allocator, ids[2]);
    id_allocator_release(allocator, ids[1]);
    ids[3] = id_allocator_acquire(allocator);
    ids[4] = id_allocator_acquire(allocator);

    ids[1] = id_allocator_acquire(allocator);
    ids[2] = id_allocator_acquire(allocator);
    ids[5] = id_allocator_acquire(allocator);
    ids[6] = id_allocator_acquire(allocator);
    ids[7] = id_allocator_acquire(allocator);
    ids[8] = id_allocator_acquire(allocator);
    ids[9] = id_allocator_acquire(allocator);

    printf("%lu\n", (unsigned long)ids[0]);
    printf("%lu\n", (unsigned long)ids[1]);
    printf("%lu\n", (unsigned long)ids[2]);
    printf("%lu\n", (unsigned long)ids[3]);
    printf("%lu\n", (unsigned long)ids[4]);
    printf("%lu\n", (unsigned long)ids[5]);
    printf("%lu\n", (unsigned long)ids[6]);
    printf("%lu\n", (unsigned long)ids[7]);
    printf("%lu\n", (unsigned long)ids[8]);
    printf("%lu\n", (unsigned long)ids[9]);

    delete_id_allocator(allocator);
}

