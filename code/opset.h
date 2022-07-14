
#ifndef _OPSET_H
#define _OPSET_H

#include "./platform.h"

extern const u32 OPSET_MASK_BIT_OFFSET;
extern const u32 OPSET_MASK_OFFSET;
extern const u32 OPSET_MASK_SIZE;

extern const u8 OPSET_SHIFT_BIT_OFFSET;
extern const u8 OPSET_SHIFT_OFFSET;
extern const u8 OPSET_SHIFT_SIZE;

#define OPSET(BOFFSET, OFFSET, SIZE) OPSET_(BOFFSET, OFFSET, SIZE)
#define OPSET_(BOFFSET, OFFSET, SIZE) (                                        \
        ((((u32)BOFFSET) << OPSET_SHIFT_BIT_OFFSET) & OPSET_MASK_BIT_OFFSET) | \
        ((((u32)OFFSET ) << OPSET_SHIFT_OFFSET    ) & OPSET_MASK_OFFSET    ) | \
        ((((u32)SIZE   ) << OPSET_SHIFT_SIZE      ) & OPSET_MASK_SIZE      )   \
    )                                                                          \

void opset_set(void *restrict, u32, const void *restrict);
void const *opset_get(const void *, const u32);
void opset_clear(void *, u32);
int opset_is_set(void *, u32);

#endif /* !_OPSET_H */

