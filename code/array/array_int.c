
#include <platform.h>
#include <stdlib.h>
#include <stdio.h>

#if 0
#define ARRAY_ELEMENT_TYPE int

#define ARRAY_ELEMENT_FORMAT_HEADER \
    char __form[16]

#define ARRAY_ELEMENT_FORMAT_PREPROCESS(FORMAT) \
    snprintf(&(__form[0]), 16, "%sd", FORMAT)

#define ARRAY_ELEMENT_FORMAT(ELEMENT, FORMATTER) \
    FORMATTER(&(__form[0]), *(ELEMENT))

#define ARRAY_TYPEDEF dArray

#define ARRAY_PREFIX

#define ARRAY_SUFFIX

#define ARRAY_NS(TOKEN) CONCAT(PREFIX, ARRAY_TYPEDEF, _, TOKEN, SUFFIX)

#else
#define ARRAY_ELEMENT_TYPE double

#define ARRAY_ELEMENT_FORMAT_HEADER \
    char __form[16]

#define ARRAY_ELEMENT_FORMAT_PREPROCESS(FORMAT) \
    snprintf(&(__form[0]), 16, "%sg", FORMAT)

#define ARRAY_ELEMENT_FORMAT(ELEMENT, FORMATTER) \
    FORMATTER(&(__form[0]), *(ELEMENT))

#define ARRAY_TYPEDEF gArray

#define ARRAY_PREFIX

#define ARRAY_SUFFIX

#define ARRAY_NS(TOKEN) CONCAT(PREFIX, ARRAY_TYPEDEF, _, TOKEN, SUFFIX)

#endif

const int ARRAY_ERR_INVALID_LIMITS = 1;
const int ARRAY_ERR_ACCESS_OUT_OF_BOUNDS = 2;
const int ARRAY_ERR_INSUFFICIENT_MEMORY = 3;

#define _ARRAY           ARRAY_TYPEDEF
#define _INIT_NOCHECK    ARRAY_NS(init_nocheck)
#define _RESERVE_NOCHECK ARRAY_NS(reserve_nocheck)
#define _FORCE_CAPACITY  ARRAY_NS(force_capacity)
#define _CHECK_LIMITS    ARRAY_NS(check_limits)
#define _EXPAND          ARRAY_NS(expand)
#define _AT_NOCHECK      ARRAY_NS(at_nocheck)
#define _NEW             ARRAY_NS(new)
#define _INIT            ARRAY_NS(init)
#define _FINALIZE        ARRAY_NS(finalize)
#define _FREE            ARRAY_NS(free)
#define _RESERVE         ARRAY_NS(reserve)
#define _INSERT          ARRAY_NS(insert)
#define _PREPEND         ARRAY_NS(prepend)
#define _APPEND          ARRAY_NS(append)
#define _AT              ARRAY_NS(at)
#define _LENGTH          ARRAY_NS(length)
#define _PRINT           ARRAY_NS(print)
#define _REPR            ARRAY_NS(repr)

typedef struct {
    counter capacity;
    counter memory;
    u8 residency;
} array_limit;

typedef struct {
    counter size;
    counter capacity;
    const array_limit *lower_limit;
    const array_limit *upper_limit;
    ARRAY_ELEMENT_TYPE *buffer;
} array;

intern array *
array_init_nocheck(
        array *,
        const array_limit *,
        const array_limit *,
        const counter,
        const counter);

intern int
array_reserve_nocheck(
        array *,
        const counter,
        const counter);

intern int
array_force_capacity(array *, const counter, const counter);

// intern int
// array_check_limits(
//         array_limit *,
//         array_limit *,
//         const counter,
//         counter *);

intern int
array_check_limits(
        array_limit *,
        array_limit *,
        const counter,
        counter *,
        const counter);

intern int
array_expand(array *, const counter, const counter);

intern void *
array_at_nocheck(const array *, const counter, const counter);



array *
array_new(const array_limit *, const array_limit *, const counter);

array *
array_init(
        array *,
        const array_limit *,
        const array_limit *,
        const counter);

array *
array_finalize(array *);

void
array_free(array *);

int
array_reserve(array *, counter, const counter);

int
array_insert(array *restrict, counter, const ARRAY_ELEMENT_TYPE *restrict, counter);

int
array_prepend(array *restrict, const ARRAY_ELEMENT_TYPE *restrict, counter);

int
array_append(array *restrict, const ARRAY_ELEMENT_TYPE *restrict, counter);

void  *
array_at(const array *, const counter, const counter);

counter
array_length(const array *);

void
array_print(const array *, FILE *, const char *);

void
array_repr(const array *, FILE *, const char *, const char *);




counter
array_length(const array *ths) {
    return ths->size;
}

intern int
array_check_limits(
        const array_limit *lower,
        const array_limit *upper,
        const counter size,
        counter *candidate,
        const counter element_size
) {
    int limits_valid;
    counter lower_memory    = (lower == NULL ? 0 : lower->memory   );
    counter lower_capacity  = (lower == NULL ? 0 : lower->capacity );
    counter lower_residency = (lower == NULL ? 0 : lower->residency);
    counter upper_memory    = (upper == NULL ? 0 : upper->memory   );
    counter upper_capacity  = (upper == NULL ? 0 : upper->capacity );

    counter lower_limit = (lower_memory + element_size - 1)/element_size;
    counter upper_limit = (upper_memory                   )/element_size;

    counter residency_limit;

    if (lower_residency > 0) {
        residency_limit = (100*size + lower_residency - 1)/lower_residency;
        if (upper_limit == 0 || upper_limit > residency_limit) {
            upper_limit = residency_limit;
        }
    }

    if (lower_capacity > 0) {
        if (lower_limit == 0 || lower_limit < lower_capacity) {
            lower_limit = lower_capacity;
        }
    }

    if (upper_capacity > 0) {
        if (upper_limit == 0 || upper_limit > upper_capacity) {
            upper_limit = upper_capacity;
        }
    }

    limits_valid = (
            lower_limit == 0 ||
            upper_limit == 0 ||
            lower_limit <= upper_limit);

    if (limits_valid && candidate) {
        if (lower_limit != 0 && *candidate < lower_limit) {
            *candidate = lower_limit;
        }

        if (upper_limit != 0 && *candidate > upper_limit) {
            *candidate = upper_limit;
        }
    }

    return (limits_valid ? 0 : ARRAY_ERR_INVALID_LIMITS);
}

intern array *
array_init_nocheck(
        array *ths,
        const array_limit *lower,
        const array_limit *upper,
        const counter cap,
        const counter element_size
) {
    ASSERT(ths);

    ths->size = 0;
    ths->capacity = 0;
    ths->lower_limit = lower;
    ths->upper_limit = upper;
    ths->buffer = NULL;

    array_reserve_nocheck(ths, cap, element_size);

    return ths;
}

array *
array_init(
        array *ths,
        const array_limit *lower,
        const array_limit *upper,
        const counter element_size
) {
    counter cap = 0;
    if (ths) {
        if (array_check_limits(lower, upper, 0, &cap, element_size)) {
            ths = array_init_nocheck(ths, lower, upper, cap, element_size);
        }
    }
    return ths;
}

array *
array_new(
        const array_limit *lower,
        const array_limit *upper,
        const counter element_size
) {
    array *ths = NULL;
    counter cap = 0;

    if (array_check_limits(lower, upper, 0, &cap, element_size)) {
        goto done;
    }

    ALLOC(ths);
    ths = array_init_nocheck(ths, lower, upper, cap, element_size);

    done:
        return ths;
}

array *
array_finalize(array *ths) {
    if (ths) { FREE(ths->buffer); }
    return ths;
}

void
array_free(array *ths) {
    ths = array_finalize(ths);
    FREE(ths);
}

int
array_reserve(array *ths, counter cap, const counter element_size) {
    int err = array_check_limits(
            ths->lower_limit, ths->upper_limit, ths->size, &cap, element_size);

    if (err) { goto done; }

    err = array_reserve_nocheck(ths, cap, element_size);

    done:
        return err;
}

intern int
array_force_capacity(
        array *ths,
        const counter cap,
        const counter element_size
) {
    u8 *buffer;
    int err;

    ASSERT(ths);

    buffer = (u8 *)(ths->buffer);
    REALLOC(buffer, cap);

    if (buffer == NULL) {
        err = ARRAY_ERR_INSUFFICIENT_MEMORY;
        goto done;
    }

    ths->buffer = (void *)(buffer);
    ths->capacity = cap;

    done:
        return err;
}

intern int
array_reserve_nocheck(
        array *ths,
        const counter cap,
        const counter element_size
) {
    int err;
    ASSERT(ths);

    if (cap <= ths->capacity) {
        err = 0;
        goto done;
    }

    err = array_force_capacity(ths, cap, element_size);

    done:
        return err;
}

intern void *
array_at_nocheck(
        const array *ths,
        const counter index,
        const counter element_size
) {
    u8 *buffer;

    ASSERT(ths);
    ASSERT(ths->buffer);
    ASSERT(index < ths->capacity);

    buffer = (u8 *)(ths->buffer);
    buffer += index*element_size;
    return (void *)buffer;
}

void *
array_at(const array *ths, const counter index, const counter element_size) {
    void *result = NULL;

#   ifndef DISABLE_BOUNDS_CHECKING
    if(index >= ths->size) { goto done; }
#   endif

    result = array_at_nocheck(ths, index, element_size);

    done:
        return result;
}

/*
 * returns:
 *     0: if x == 0
 * otherwise:
 *     returns the smallest 2^i for i in NN such that 2^i >= x
 */
intern inline u64
_double_ceil64(u64 x) {
    --x;
    x |= x >>  1;
    x |= x >>  2;
    x |= x >>  4;
    x |= x >>  8;
    x |= x >> 16;
    x |= x >> 32;
    ++x;

    return x;
}

intern int
array_expand(array *ths, const counter new_size, const counter element_size) {
    int err;
    counter cap = new_size;

    err = array_check_limits(
            ths->lower_limit, ths->upper_limit, new_size, &cap, element_size);
    if (err) { goto done; }

    /* can not expand array to [new_size]: limited by user-provided limits */
    if (cap < new_size) {
        array_reserve_nocheck(ths, cap, element_size);
        ths->size = cap;
        err = ARRAY_ERR_INSUFFICIENT_MEMORY;
        goto done;
    }

    /*
     * can expand array to at least [new_size]:
     *     try to expand to 2^i elements
     */
    if (cap >= new_size) {
        cap = (counter)_double_ceil64((u64)cap);
        err = array_check_limits(ths->lower_limit,
                                 ths->upper_limit,
                                 new_size,
                                 &cap,
                                 element_size);
        if (err) { goto done; }
    }

    array_reserve_nocheck(ths, cap, element_size);
    ths->size = new_size;

    done:
        return err;
}

int
array_insert(
        array *restrict ths,
        counter index,
        const ARRAY_ELEMENT_TYPE *restrict data,
        counter count) {

    int err;
    ARRAY_ELEMENT_TYPE *start;
    ARRAY_ELEMENT_TYPE *end;
    counter displaced_bytes;
    counter insert_bytes;
    counter num_displaced;

#   ifndef DISABLE_BOUNDS_CHECKING
    if(index > ths->size) {
        err = ARRAY_ERR_ACCESS_OUT_OF_BOUNDS;
        goto done;
    }
#   endif

    num_displaced = ths->size - index;
    array_expand(ths, ths->size + count);

    if (num_displaced > 0) { goto set_end; }
    if (count > 0) { goto set_start; }

    set_end:
        end = array_at_nocheck(ths, index + count);
        displaced_bytes = num_displaced*sizeof(ARRAY_ELEMENT_TYPE);

    set_start:
        start = array_at_nocheck(ths, index);
        insert_bytes = count*sizeof(ARRAY_ELEMENT_TYPE);

    if (num_displaced > 0) {
        if (num_displaced <= count) {
            memcpy(end, start, displaced_bytes);
        } else {
            memmove(end, start, displaced_bytes);
        }
    }

    if (count > 0) {
        memcpy(start, data, insert_bytes);
    }

    err = 0;

    done:
        return err;
}

int
array_prepend(array *restrict ths, const ARRAY_ELEMENT_TYPE *restrict data, counter count) {
    return array_insert(ths, 0, data, count);
}

int
array_append(array *restrict ths, const ARRAY_ELEMENT_TYPE *restrict data, counter count) {
    return array_insert(ths, ths->size, data, count);
}

int
array_remove(array *ths, counter index, counter count) {
    int err;

    ARRAY_ELEMENT_TYPE *start;
    ARRAY_ELEMENT_TYPE *end;
    counter remaining_bytes;
    counter new_capacity;
    counter new_size;

#   ifndef DISABLE_BOUNDS_CHECKING
    if(index >= ths->size) {
        err = ARRAY_ERR_ACCESS_OUT_OF_BOUNDS;
        goto done;
    }
    if(index + count > ths->size) {
        err = ARRAY_ERR_ACCESS_OUT_OF_BOUNDS;
        goto done;
    }
#   endif

    if (count == 0) {
        err = 0;
        goto done;
    }

    new_size = ths->size - count;
    new_capacity = ths->capacity;

    if (4*new_size > new_capacity) {
        new_capacity = (counter)_double_ceil64((u64)(new_size << 1));
        err = array_check_limits(
                ths->lower_limit, ths->upper_limit, new_size, &new_capacity, element_size);
        if (err) { goto done; }
    }

    start = array_at_nocheck(ths, index);
    remaining_bytes = (ths->size - index - count)*sizeof(ARRAY_ELEMENT_TYPE);

    if (remaining_bytes > 0) {
        end = array_at_nocheck(ths, index + count);

        /*
         *   [0 1 2 . . . . 7 8 9]
         *                  |----| <- size - index - count
         *          |------|       <- count
         */
        if (ths->size - index - count <= count) {
            memcpy(start, end, remaining_bytes);
        } else {
            memmove(start, end, remaining_bytes);
        }
    }

    ths->size = new_size;

    if (ths->capacity > new_capacity) {
        array_force_capacity(ths, new_capacity);
    }

    err = 0;

    done:
        return err;
}

void
array_print(const array *ths, FILE *file, const char *format) {
#   undef _formatter
#   define _formatter(F, E) fprintf(file, (F), (E))
    counter n;
    const ARRAY_ELEMENT_TYPE *p;
    ARRAY_ELEMENT_FORMAT_HEADER;

    if (format == NULL) { format = "%"; }

    ARRAY_ELEMENT_FORMAT_PREPROCESS(format);

    fprintf(file, "[");

    n = ths->size;
    p = array_at(ths, 0);

    if (n--) {
        ARRAY_ELEMENT_FORMAT(p, _formatter);
        ++p;

        for ( ; n--; ++p) {
            fprintf(file, ", ");
            ARRAY_ELEMENT_FORMAT(p, _formatter);
        }
    }

    fprintf(file, "]");
#   undef _formatter
}

void
array_repr(
        const array *ths,
        FILE *file,
        const char *format,
        const char *empty
) {
#   undef _formatter
#   define _formatter(F, E) fprintf(file, (F), (E))
    counter n;
    const ARRAY_ELEMENT_TYPE *p;
    ARRAY_ELEMENT_FORMAT_HEADER;

    if (format == NULL) { format = "%"; }

    fprintf(file,
            "%2u/%2u:",
            (unsigned int)ths->size,
            (unsigned int)ths->capacity);

    ARRAY_ELEMENT_FORMAT_PREPROCESS(format);

    n = ths->size;
    p = array_at(ths, 0);

    for ( ; n--; ++p) {
        fprintf(file, " ");
        ARRAY_ELEMENT_FORMAT(p, _formatter);
    }

    n = ths->capacity;
    for ( ; (n--) > ths->size; ) {
        fprintf(file, " %s", (empty ? empty : "xx"));
    }
#   undef _formatter
}



MAIN {
#   undef check_err
#   define check_err if(err) goto done

    unsigned int N = 10;
    int i, err;
    ARRAY_ELEMENT_TYPE data[4];

    array_limit lower = {0};
    lower.memory = 33;

    array_limit upper = {0};
    upper.memory = sizeof(ARRAY_ELEMENT_TYPE)*14 + 4;
    // upper.capacity = 15;

    // array *a = array_new(NULL, NULL);
    array *a = array_new(&lower, &upper);
    array_print(a, stdout, NULL);
    printf("\n");

    for (i=0; i<N; ++i) {
        data[0] = (ARRAY_ELEMENT_TYPE)i;
        err = array_prepend(a, &(data[0]), 1); check_err;
        array_print(a, stdout, NULL);
        printf("\n");
    }

    array_reserve(a, 9);
    array_print(a, stdout, NULL);
    printf("\n");

    for (; i<10; ++i) {
        data[0] = (ARRAY_ELEMENT_TYPE)i;
        array_prepend(a, &(data[0]), 1);
        array_print(a, stdout, NULL);
        printf("\n");
    }

    printf("\n");

    for (i=0, N=array_length(a); i<N; ++i) {
        *array_at(a, i) = (ARRAY_ELEMENT_TYPE)i;
        array_print(a, stdout, NULL);
        printf("\n");
    }

    data[0] = (ARRAY_ELEMENT_TYPE)1000;
    data[1] = (ARRAY_ELEMENT_TYPE)2000;
    data[2] = (ARRAY_ELEMENT_TYPE)3000;
    data[3] = (ARRAY_ELEMENT_TYPE)4000;

    array_insert(a, 4, &(data[0]), 4);
    array_print(a, stdout, NULL);
    printf("\n");

    array_remove(a, 4, 10);
    array_print(a, stdout, "% 4.3");
    printf("\n");

    array_free(a);

    done:
        if (err == ARRAY_ERR_INVALID_LIMITS) {
            fprintf(stderr, "Invalid array limits.\n");
        }

        if (err == ARRAY_ERR_ACCESS_OUT_OF_BOUNDS) {
            fprintf(stderr, "Array access out of bounds.\n");
        }

    exit(err);

#   undef check_err
}
