
#ifndef MEM_STACK_H
#define MEM_STACK_H

#include "platform.h"

#ifndef MEM_STACK_INITIAL_CAPACITY
#   define MEM_STACK_INITIAL_CAPACITY 32
#endif

struct mem_stack_marker;
struct mem_stack;

struct mem_stack {
    ptr buffer;
    counter size;
    counter capacity;
    struct mem_stack_marker *marker_list;
};

void delete_mem_stack(struct mem_stack *);
struct mem_stack *mem_stack_finalize(struct mem_stack *);
struct mem_stack *mem_stack_init(struct mem_stack *);
void mem_stack_pop(struct mem_stack *);
ptr mem_stack_push(struct mem_stack *, counter);
struct mem_stack *new_mem_stack(void);

#endif /* !MEM_STACK_H */

