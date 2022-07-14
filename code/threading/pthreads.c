
#include <pthread.h>

#include "../platform.h"

STRUCT_DECL(thread);
STRUCT_DEF(thread) {
    pthread_t super;
};

