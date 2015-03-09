
#include "platform.h"

#include "mem_stack.h"

struct mem_stack_marker {
    struct mem_stack_marker *prev;
    counter end_offset;
};

void
delete_mem_stack(struct mem_stack *ths) {
    FREE(mem_stack_finalize(ths));
}

struct mem_stack *
mem_stack_finalize(struct mem_stack *ths) {
    struct mem_stack_marker *a = NULL;
    struct mem_stack_marker *b = NULL;

    for(
        a = ths->marker_list,
        b = (a == NULL ? NULL : a->prev);

        a != NULL;

        a = b,
        b = (a == NULL ? NULL : a->prev)
    ) {
        FREE(a);
    }

    FREE(ths->buffer);

    return ths;
}

struct mem_stack *
mem_stack_init(struct mem_stack *ths) {
    u8 *buffer;
    ALLOC(buffer, MEM_STACK_INITIAL_CAPACITY);
    ths->buffer = buffer;
    ths->size = 0;
    ths->capacity = MEM_STACK_INITIAL_CAPACITY;
    ths->marker_list = NULL;
    return ths;
}

void
mem_stack_pop(struct mem_stack *ths) {
    struct mem_stack_marker *marker;

    marker = ths->marker_list;
    ths->marker_list = marker->prev;

    FREE(marker);
}

ptr
mem_stack_push(struct mem_stack *ths, counter size) {
    ptr result;
    u8 *buffer;
    u8 needs_resize;
    struct mem_stack_marker *marker;
    counter new_size;

    new_size = ths->size + size;
    needs_resize = 0;
    while(new_size > ths->capacity) {
        ths->capacity *= 2;
        needs_resize = 1;
    }

    if(needs_resize) {
        buffer = ths->buffer;
        REALLOC(buffer, ths->capacity);
        ths->buffer = buffer;
    }

    result = ths->buffer;

    ALLOC(marker);
    marker->prev = ths->marker_list;
    marker->end_offset = size;
    if(marker->prev != NULL) {
        marker->end_offset += marker->prev->end_offset;
        result += marker->prev->end_offset;
    }
    ths->marker_list = marker;

    return result;
}

struct mem_stack *
new_mem_stack() {
    struct mem_stack *ths;
    ALLOC(ths);
    return mem_stack_init(ths);
}

