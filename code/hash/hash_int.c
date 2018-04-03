
#include <platform.h>
#include <stdlib.h>
#include <stdio.h>

intern const counter HASH_MIN_CAPACITY =  2;
intern const counter HASH_MIN_MEMORY   = 64;

intern const u8 HASH_FLAGS_FLIP = (0x1 << 0);

typedef struct {
    counter capacity;
    u8 residency;
} hash_limit;

typedef struct {
    int key;
    int val;
    u8 occupied;
} bin;

typedef struct {
    counter size;
    counter capacity;
    hash_limit lower_limit;
    hash_limit upper_limit;
    bin *bins;
    u8 flags;
} hash;

hash *
hash_init(hash *ths, hash_limit lower, hash_limit upper) {
    if (!ths) goto done;

    ths->size = 0;
    ths->capacity = 0;
    ths->lower_limit = lower;
    ths->upper_limit = upper;
    ths->bins = NULL;
    ths->flags = 0x0;

    done:
        return ths;
}

hash *
hash_new(hash_limit lower, hash_limit upper) {
    hash *ths;
    ALLOC(ths);
    return hash_init(ths, lower, upper);
}

hash *
hash_finalize(hash *ths) {
    if (!ths) goto done;

    FREE(ths->bins);

    done:
        return ths;
}

void
hash_free(hash *ths) {
    ths = hash_finalize(ths);
    FREE(ths);
}

counter
hash_seek_primary(hash *ths, int key) {
    counter result = key;
    result %= ths->capacity;
    return result;
}

counter
hash_seek(hash *ths, int key) {
    counter result;
    counter offset, of;
    counter shift , sh;
    counter candidate;

    result = hash_seek_primary(ths, key);

    for (
        offset = 0, shift = 0;
        offset < ths->capacity;
        offset += (++shift)
    ) {
        for(
            of = offset, sh = shift, of += (sh++);
            of < ths->capacity;
            of += (sh++)
        ) {
            candidate = (result + of) % ths->capacity;
            if (ths->bins[candidate].occupied &&
                ths->bins[candidate].key != key) {
                continue;
            }

            result = candidate;
            goto done;
        }
    }

    ASSERT(0);

    done:
        return result;
}

void
hash_rehash(hash *ths, counter old_cap) {
    counter tmp_index;
    counter bin_index;
    counter new_index;

    tmp_index = 0;

    for (bin_index = 0; bin_index < old_cap; ++bin_index) {
    }
}

void
hash_reserve(hash *ths, counter cap) {
    counter acap;
    bin *bins;

#undef update_cap
#define update_cap(X) \
    do { acap = (X); if (cap < acap) { cap = acap; } } while(0)

    update_cap(ths->lower_limit.capacity);
    update_cap(HASH_MIN_CAPACITY);
    update_cap((HASH_MIN_MEMORY + sizeof(bin) - 1)/sizeof(bin));

// TODO
//     if (cap <= ths->capacity) { return; }
// 
//     bins = ths->bins;
//     REALLOC(bins, cap);
//     ths->bins = bins;
//     for(
//             acap = ths->capacity,
//             bins = ths->bins + ths->capacity;
// 
//             acap < cap;
// 
//             ++bins,
//             ++acap
//     ) {
//         bins->occupied
//     }
//     ths->capacity = cap;

#undef update_cap
}

// void
// hash_set

void probe(unsigned int N) {
    unsigned int offset, of;
    unsigned int shift , sh;

    printf("PROBE (%u)\n", N);

    for (offset = 0, shift = 0; offset < N; offset += (++shift)) {
        for(of = offset, sh = shift, of += (sh++); of < N; of += (sh++)) {
            printf("%u, ", of);
        }
    }
    printf("\n");
}

MAIN {
    unsigned int N;
    sscanf(argv[1], "%u", &N);
    probe(N);
}

