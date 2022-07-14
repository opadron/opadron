
#include <stdio.h>

#include "platform.h"
#include "c_buffer.h"

STRUCT_DECL(c_buffer);
STRUCT_DEF(c_buffer) {
    hdl head, tail;
};

intern ptr c_buffer_init(ptr ths_) {
    c_buffer *ths = ths_;

    if (ths) {
        ths->head = NULL;
        ths->tail = NULL;
    }

    return ths;
}

intern ptr c_buffer_new() {
    c_buffer *ths;
    ALLOC(ths);

    if (ths) {
        ths = c_buffer_init(ths);
    }

    return ths;
}

intern ptr c_buffer_finalize(ptr ths_) {
    c_buffer *ths = ths_;
    hdl a, b;

    if (ths) {
        for (a = ths->head;
             a;
             a = b)
        {
            b = *a;
            FREE(a);
        }
    }

    return ths;
}

void c_buffer_free(ptr ths_) {
    FREE(c_buffer_finalize(ths_));
}

ptr c_buffer_push(ptr ths_, ptr x, counter count, counter size) {
    c_buffer *ths = ths_;
    u8 *buf;
    const counter sizeof_ptr = sizeof(ptr);

    if (!ths) {
        ths = c_buffer_new();
    }

    ALLOC(buf, sizeof_ptr + count*size);
    COPY(buf + sizeof_ptr, x, count, size);

    x = buf;
    *(void **)x = NULL;

    if (ths->tail) {
        *ths->tail = x;
    }

    ths->tail = x;

    if (!ths->head) {
        ths->head = ths->tail;
    }

    return ths;
}

ptr c_buffer_pop(ptr ths_, ptr x, counter count, counter size) {
    c_buffer *ths = ths_;
    const counter sizeof_ptr = sizeof(ptr);
    u8 do_free = 1;
    ptr buf;

    if (ths) {
        if (ths->head && ths->tail) {
            buf = ths->head;
            ths->head = *ths->head;
            COPY(x, buf + sizeof_ptr, count, size);
            FREE(buf);

            if (ths->head) {
                do_free = 0;
            } else {
                ths->tail = ths->head;
            }
        }

        if (do_free) {
            c_buffer_free(ths);
            ths = NULL;
        }
    }

    return ths;
}

