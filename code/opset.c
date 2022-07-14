
#include <string.h>

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "./platform.h"
#include "opset.h"

const u32 OPSET_MASK_BIT_OFFSET = 0x000000FF;
const u32 OPSET_MASK_OFFSET     = 0x0000FF00;
const u32 OPSET_MASK_SIZE       = 0x00FF0000;

const u8 OPSET_SHIFT_BIT_OFFSET =  0;
const u8 OPSET_SHIFT_OFFSET     =  8;
const u8 OPSET_SHIFT_SIZE       = 16;

void opset_set(void *restrict ths, u32 op, const void *restrict val) {
    u8 boffset = ((op & OPSET_MASK_BIT_OFFSET) >> OPSET_SHIFT_BIT_OFFSET);
    u8 offset  = ((op & OPSET_MASK_OFFSET    ) >> OPSET_SHIFT_OFFSET    );
    u8 size    = ((op & OPSET_MASK_SIZE      ) >> OPSET_SHIFT_SIZE      );

    *((u64 *)ths) |= (1 << boffset);

    if(size) {
        ths += offset;
        memcpy(ths, val, size);
    }
}

void const *opset_get(const void *ths, const u32 op) {
    const u8 boffset = (op & OPSET_MASK_BIT_OFFSET) >> OPSET_SHIFT_BIT_OFFSET;
    const u8 offset  = (op & OPSET_MASK_OFFSET    ) >> OPSET_SHIFT_OFFSET    ;

    return (
        (*((u64 *)ths) & (1 << boffset))
          ? (ths + offset)
          : NULL
    );
}

void opset_clear(void *ths, u32 op) {
    u8 boffset = (op & OPSET_MASK_BIT_OFFSET) >> OPSET_SHIFT_BIT_OFFSET;
    *((u64 *)ths) &= ~(1 << boffset);
}

int opset_is_set(void *ths, u32 op) {
    u8 boffset = (op & OPSET_MASK_BIT_OFFSET) >> OPSET_SHIFT_BIT_OFFSET;
    return (
        (*((u64 *)ths) & (1 << boffset))
          ? 1 : 0
    );
}
