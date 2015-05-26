
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <platform.h>
#include <trie.h>

local struct trie *table;
local char buffer[80];
local counter buffer_size;

intern void init() {
    RUN_ONCE_OR_RETURN;

    buffer_size = 0;
    table = trie_new('\0', NULL);
}

intern void finalize() {
    RUN_ONCE_OR_RETURN;

    trie_delete(table, NULL);
}

intern enum trie_iter_code on_push(struct trie *ths, char key) {
    buffer[buffer_size] = key;
    ++buffer_size;
    return TRIE_ITER_CONTINUE;
}

intern enum trie_iter_code on_entry(struct trie *ths, ptr value) {
    counter size;

    for(size=0; size<buffer_size; ++size) {
        putchar(buffer[size]);
    }
    putchar('\n');

    return TRIE_ITER_CONTINUE;
}

intern enum trie_iter_code on_pop(struct trie *ths) {
    if(buffer_size > 0) {
        --buffer_size;
    }

    return TRIE_ITER_CONTINUE;
}

MAIN {
    init();

    char key[20];
    char *keyPtr = (&key[0]);

    strcpy(keyPtr, "hello");
    trie_set(table, keyPtr, NULL);

    strcpy(keyPtr, "hi");
    trie_set(table, keyPtr, NULL);

    strcpy(keyPtr, "help");
    trie_set(table, keyPtr, NULL);

    trie_iter(table, on_push, on_entry, on_pop);

    finalize();
}

