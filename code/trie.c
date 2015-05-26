
#include "trie.h"

#include <platform.h>

/* private headers */
intern enum trie_iter_code trie_iter_default_push_callback(struct trie *, char);
intern enum trie_iter_code trie_iter_default_entry_callback(struct trie *, ptr);
intern enum trie_iter_code trie_iter_default_pop_callback(struct trie *);
intern enum trie_iter_code __trie_iter__(
    struct trie *,
    enum trie_iter_code (*)(struct trie *, char),
    enum trie_iter_code (*)(struct trie *, ptr),
    enum trie_iter_code (*)(struct trie *)
);


struct trie *trie_new(
    char character,
    struct trie *parent
) {
    struct trie *result;
    ALLOC(result);

    result->parent = parent;
    result->character = character;
    result->value = NULL;
    result->children = NULL;

    /* insert into parent's children */
    if(parent == NULL) return result;

    struct trie *sibling_a = parent->children;
    if(sibling_a == NULL) {
        parent->children = result;
        parent->children->siblings = parent->children;
        return result;
    }

    struct trie *sibling_b = sibling_a->siblings;
    if(sibling_a == sibling_b) {
        sibling_a->siblings = result;
        result->siblings = sibling_a;
        parent->children = result;
        return result;
    }

    sibling_a->siblings = result;
    result->siblings = sibling_b;
    return result;
}

void trie_delete(struct trie *ths, void (*destructor)(ptr)) {
    if(destructor == NULL) {
        destructor = &free;
    }

    struct trie *start;
    struct trie *iter;

    start = ths->children;
    if(start != NULL) {
        u8 first = 1;
        for(
            iter = start;
            (first || iter != start);
            iter = iter->siblings
        ) {
            first = 0;
            trie_delete(iter, destructor);
        }
    }

    if(ths->value_set) {
        destructor(ths->value);
    }

    free(ths);
}

void trie_set(struct trie *ths, char *key, ptr value) {
    if(*key == '\0') {
        ths->value_set = 1;
        ths->value = value;
        return;
    }

    u8 found = 0;
    struct trie *start;
    struct trie *iter;

    start = ths->children;

    if(start != NULL) {
        u8 first = 1;
        for(
            iter = start;
            (first || iter != start);
            iter = iter->siblings
        ) {
            first = 0;
            if(iter->character == *key) {
                found = 1;
                break;
            }
        }
    }

    if(!found) {
        iter = trie_new(*key, ths);
    }

    trie_set(iter, key + 1, value);
}

intern enum trie_iter_code
trie_iter_default_push_callback(struct trie *trie, char character) {
    return TRIE_ITER_CONTINUE;
}

intern enum trie_iter_code
trie_iter_default_entry_callback(struct trie *trie, ptr value) {
    return TRIE_ITER_CONTINUE;
}

intern enum trie_iter_code
trie_iter_default_pop_callback(struct trie *trie) {
    return TRIE_ITER_CONTINUE;
}

intern enum trie_iter_code
__trie_iter__(
    struct trie *ths,
    enum trie_iter_code (*push_callback)(struct trie *, char),
    enum trie_iter_code (*entry_callback)(struct trie *, ptr),
    enum trie_iter_code (*pop_callback)(struct trie *)
) {
    enum trie_iter_code code;

    code = push_callback(ths, ths->character);

    if(code == TRIE_ITER_CONTINUE) {
        if(ths->value_set) {
            code = entry_callback(ths, ths->value);
        }

        if(code == TRIE_ITER_CONTINUE) {
            struct trie *start;
            struct trie *iter;

            start = ths->children;

            if(start != NULL) {
                u8 first = 1;
                for(
                    iter = start;
                    (first || iter != start);
                    iter = iter->siblings
                ) {
                    first = 0;
                    code = __trie_iter__(
                        iter,
                        push_callback,
                        entry_callback,
                        pop_callback
                    );

                    if(code == TRIE_ITER_PRUNE) {
                        code = TRIE_ITER_CONTINUE;
                    }

                    if(code == TRIE_ITER_CONTINUE) {
                        continue;
                    }

                    if(code == TRIE_ITER_BREAK) {
                        break;
                    }
                }
            }
        }
    }

    enum trie_iter_code code2 = pop_callback(ths);

    u8 escalate_code = (
        code != code2
        && (
            code == TRIE_ITER_CONTINUE
            ||
            code2 == TRIE_ITER_BREAK
        )
    );

    if(escalate_code) {
        code = code2;
    }

    return code;
}

void
trie_iter(
    struct trie *ths,
    enum trie_iter_code (*push_callback)(struct trie *, char),
    enum trie_iter_code (*entry_callback)(struct trie *, ptr),
    enum trie_iter_code (*pop_callback)(struct trie *)
) {
    if(push_callback == NULL) {
        push_callback = trie_iter_default_push_callback;
    }

    if(entry_callback == NULL) {
        entry_callback = trie_iter_default_entry_callback;
    }

    if(pop_callback == NULL) {
        pop_callback = trie_iter_default_pop_callback;
    }

    struct trie *start;
    struct trie *iter;

    start = ths->children;
    if(start != NULL) {
        u8 first = 1;
        for(
            iter = start;
            (first || iter != start);
            iter = iter->siblings
        ) {
            first = 0;
            enum trie_iter_code code = __trie_iter__(
                iter,
                push_callback,
                entry_callback,
                pop_callback
            );

            if(code == TRIE_ITER_PRUNE) {
                code = TRIE_ITER_CONTINUE;
            }

            if(code == TRIE_ITER_CONTINUE) {
                continue;
            }

            if(code == TRIE_ITER_BREAK) {
                break;
            }
        }
    }
}

struct trie *
trie_find_child(struct trie *ths, char key) {
    struct trie *start;
    struct trie *iter;
    struct trie *result = NULL;

    start = ths->children;
    if(start != NULL) {
        u8 first = 1;
        for(
            iter = start;
            (first || iter != start);
            iter = iter->siblings
        ) {
            first = 0;

            if(iter->character == key) {
                result = iter;
                break;
            }
        }
    }

    return result;
}

