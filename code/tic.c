
#include <stdlib.h>
#include <time.h>

#include "tic.h"
 
#ifdef __MACH__ /* emulate clock_gettime on mac os */
#   include <mach/clock.h>
#   include <mach/mach.h>

#   define clock_gettime(A, B) clock_gettime_emulated(B)

    static
    void
    clock_gettime_emulated(struct timespec *ts) {
        clock_serv_t cclock;
        mach_timespec_t mts;

        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);

        ts->tv_sec = mts.tv_sec;
        ts->tv_nsec = mts.tv_nsec;
    }

#endif

static const int ONE_BILLION = 1000000000;

static
inline
int
timespec_diff(
                   struct timespec *c,
    register const struct timespec *a,
             const struct timespec *bb
) {
    struct timespec b0;
    b0.tv_nsec = bb->tv_nsec;
    b0.tv_sec  = bb->tv_sec;

    register struct timespec *b = &b0;

    int carry_seconds;

    /* Perform the carry for the later subtraction by updating b. */
    if(a->tv_nsec < b->tv_nsec) {
        carry_seconds = (b->tv_nsec - a->tv_nsec)/ONE_BILLION + 1;
        b->tv_nsec -= ONE_BILLION*carry_seconds;
        b->tv_sec += carry_seconds;
    }

    if(a->tv_nsec - b->tv_nsec > ONE_BILLION) {
        carry_seconds = (a->tv_nsec - b->tv_nsec)/ONE_BILLION;
        b->tv_nsec += ONE_BILLION*carry_seconds;
        b->tv_sec -= carry_seconds;
    }

    /* Compute the time remaining to wait. tv_nsec is certainly positive. */
    c->tv_sec = a->tv_sec - b->tv_sec;
    c->tv_nsec = a->tv_nsec - b->tv_nsec;

    /* Return 1 if result is negative. */
    return a->tv_sec < b->tv_sec;
}

 
struct tt_clock_stack;
struct tt_clock_stack {
    struct timespec ts;
    struct tt_clock_stack *prev;
};
 
tt_clock *
tt_init(tt_clock *ths) {
    ths->stack = NULL;
    return ths;
}

tt_clock *
tt_clear(tt_clock *ths) {
    struct tt_clock_stack *tmp = ths->stack;
    if(tmp) {
        ths->stack = ths->stack->prev;
        free(tmp);
        tt_clear(ths);
    }

    return ths;
}

tt_clock *
tt_alloc() {
    return tt_init(malloc(sizeof(tt_clock)));
}

void
tt_free(tt_clock *ths) {
    free(tt_clear(ths));
}

void
tt_tic(tt_clock *ths) {
    struct tt_clock_stack *tmp = malloc(sizeof(*tmp));
    clock_gettime(CLOCK_REALTIME, &(tmp->ts));
    tmp->prev = ths->stack;
    ths->stack = tmp;
}

struct timespec *
tt_tstoc(tt_clock *ths, struct timespec *retval) {
    struct timespec ts;
    struct tt_clock_stack *tmp;

    clock_gettime(CLOCK_REALTIME, &ts);
    timespec_diff(retval, &ts, &(ths->stack->ts));

    tmp = ths->stack;
    ths->stack = ths->stack->prev;

    free(tmp);

    return retval;
}

double
tt_toc(tt_clock *ths) {
    struct timespec ts;
    tt_tstoc(ths, &ts);

    return ((double)ts.tv_sec) + ((double)ts.tv_nsec)/ONE_BILLION;
}

