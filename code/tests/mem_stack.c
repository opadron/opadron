
#include <stdlib.h>
#include <stdio.h>

#include "platform.h"
#include "mem_stack.h"

void
push_and_print(struct mem_stack *stack, u8 level) {
    counter local_size = 1 << level;
    counter global_size = (1 << (level + 1)) - 1;

    u8* x;
    u8 count;
    for(
        count = 0,
        x = mem_stack_push(stack, local_size*sizeof(*x));

        count < local_size;

        ++count,
        ++x
    ) {
        *x = count;
    }

    x = stack->buffer;
    printf("%u", (unsigned)(*x));

    for(
        ++x,
        count=1;

        count < global_size;

        ++count,
        ++x
    ) {
        printf(" %u", (unsigned)(*x));
    }

    printf("\n");

    if(level < 4) {
        push_and_print(stack, level + 1);
    }
}

int
main(int argc, char **argv) {
    struct mem_stack *stack = new_mem_stack();
    push_and_print(stack, 0);
    delete_mem_stack(stack);
    return EXIT_SUCCESS;
}

