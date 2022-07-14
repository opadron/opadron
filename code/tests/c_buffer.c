
#include <stdio.h>
#include <time.h>

#include "platform.h"
#include "c_buffer.h"

MAIN {
    void *buf = NULL;
    int x;

    srand(time(0));
    do {
        x = rand() % 10;
        buf = c_buffer_push(buf, &x, 1, sizeof(x));
    } while (x);

    while (buf) {
        buf = c_buffer_pop(buf, &x, 1, sizeof(x));
        printf("%d\n", x);
    }
}

