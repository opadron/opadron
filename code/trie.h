
#ifndef _TRIE_H
#define _TRIE_H

#include <platform.h>

struct trie {
    char character;
    u8 value_set;
    ptr value;
    struct trie *parent;
    struct trie *siblings;
    struct trie *children;
};

enum trie_iter_code {
    TRIE_ITER_CONTINUE,
    TRIE_ITER_PRUNE,
    TRIE_ITER_BREAK
};

extern struct trie *trie_new(char, struct trie *);
extern void trie_delete(struct trie *, void (*)(ptr));
extern void trie_set(struct trie *, char *, ptr);
extern void trie_iter(
    struct trie *,
    enum trie_iter_code (*pop_callback)(struct trie *, char),
    enum trie_iter_code (*entry_callback)(struct trie *, ptr),
    enum trie_iter_code (*push_callback)(struct trie *)
);
extern struct trie *trie_find_child(struct trie *, char);

#endif /* !_TRIE_H */

