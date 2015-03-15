
#include "platform.h"

#include "id_allocator.h"

struct id_allocator_free_list;

struct id_allocator_free_list {
    struct id_allocator_free_list *next;
    counter value;
};


void
delete_id_allocator(struct id_allocator *ths) {
    FREE(id_allocator_finalize(ths));
}

struct id_allocator *
id_allocator_finalize(struct id_allocator *ths) {
    struct id_allocator_free_list *a = NULL;
    struct id_allocator_free_list *b = NULL;

    for(
        a = ths->free_list_head,
        b = (a == NULL ? NULL : a->next);

        a != NULL;

        a = b,
        b = (a == NULL ? NULL : a->next)
    ) {
        FREE(a);
    }

    return ths;
}

struct id_allocator *
id_allocator_init(struct id_allocator *ths) {
    ths->free_list_head = NULL;
    ths->free_list_tail = NULL;
    ths->max_id = 0;

    return ths;
}

counter
id_allocator_acquire(struct id_allocator *ths) {
    counter result;
    struct id_allocator_free_list *tmp;

    result = ths->max_id;

    if(ths->free_list_head == NULL) {
        ++ths->max_id;
    } else {
        tmp = ths->free_list_head;

        result = tmp->value;
        if(ths->free_list_head == ths->free_list_tail) {
            ths->free_list_tail = NULL;
        }

        ths->free_list_head = ths->free_list_head->next;

        FREE(tmp);
    }

    return result;
}

void
id_allocator_release(struct id_allocator *ths, counter id) {
    struct id_allocator_free_list *tmp;

    ASSERT(id < ths->max_id);

    ALLOC(tmp);
    tmp->next = NULL;
    tmp->value = id;

    if(ths->free_list_head == NULL) {
        ths->free_list_head = tmp;
    } else {
        ths->free_list_tail->next = tmp;
    }

    ths->free_list_tail = tmp;
}

struct id_allocator *
new_id_allocator() {
    struct id_allocator *ths;
    ALLOC(ths);
    return id_allocator_init(ths);
}

