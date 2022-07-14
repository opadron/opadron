
#ifndef C_BUFFER_H
#define C_BUFFER_H

#include "platform.h"

extern void c_buffer_free(ptr);
extern ptr c_buffer_push(ptr, ptr, counter, counter);
extern ptr c_buffer_pop(ptr, ptr, counter, counter);

#endif /* !C_BUFFER_H */

