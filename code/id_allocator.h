
#ifndef _ID_ALLOCATOR_H
#define _ID_ALLOCATOR_H

#include "platform.h"

struct id_allocator {
    struct id_allocator_free_list *free_list_head;
    struct id_allocator_free_list *free_list_tail;
    counter max_id;
};

extern void delete_id_allocator(struct id_allocator *);
extern struct id_allocator *id_allocator_finalize(struct id_allocator *);
extern struct id_allocator *id_allocator_init(struct id_allocator *);
extern counter id_allocator_acquire(struct id_allocator *);
extern void id_allocator_release(struct id_allocator *, counter);
extern struct id_allocator *new_id_allocator(void);

#endif /* !_ID_ALLOCATOR_H */

