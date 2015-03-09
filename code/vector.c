
#include <stdlib.h>

#include "platform.h"

#ifndef VECTOR_INITIAL_CAPACITY
#   define VECTOR_INITIAL_CAPACITY 32
#endif

struct vector {
    counter size;
    counter capacity;
    size_t element_size;
    ptr buffer;
};

struct vector *
vector_init(struct vector *ths, size_t element_size) {
    u8 *buffer;
    counter initial_capacity;

    initial_capacity = (VECTOR_INITIAL_CAPACITY/element_size) || 0;
    ALLOC(buffer, initial_capacity);

    ths->size = 0;
    ths->capacity = initial_capacity;
    ths->element_size = element_size;
    ths->buffer = buffer;
}

struct vector *
new_vector() {
    struct vector *ths;
    ALLOC(ths);
    return vector_init(ths);
}

struct vector *
vector_finalize(struct vector *ths) {
    FREE(ths->buffer);
    return ths;
}

void
delete_vector(struct vector *ths) {
    FREE(vector_finalize(ths));
}

void vector_pop(struct vector *);
ptr vector_push(struct vector *, counter);



