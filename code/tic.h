
#ifndef TIC_H
#define TIC_H

#include <stdlib.h>
#include <time.h>

struct tt_clock;
typedef struct tt_clock tt_clock;
struct tt_clock {
    struct tt_clock_stack *stack;
};


tt_clock *tt_init (tt_clock *);
tt_clock *tt_clear(tt_clock *);
tt_clock *tt_alloc(void      );
void      tt_free (tt_clock *);

void             tt_tic  (tt_clock *);
struct timespec *tt_tstoc(tt_clock *, struct timespec *);
double           tt_toc  (tt_clock *);

#endif /* !TIC_H */

