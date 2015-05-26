
#include <stdlib.h>
#include <stdio.h>

#include <tic.h>

int
main(int argc, char **argv) {

    volatile int i, j;

    tt_clock clock;
    tt_init(&clock);

    tt_tic(&clock);
    for(i=0; i<10; ++i) {

        tt_tic(&clock);
        for(j=0; j<100000000; ++j) { /* void */ }
        printf("Step: %g\n", tt_toc(&clock)); fflush(stdout);

    }
    printf("Total: %g\n", tt_toc(&clock)); fflush(stdout);

    tt_clear(&clock);
}

