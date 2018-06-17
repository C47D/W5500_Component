#include "common.h"

struct W5500 this;

bool compare_arrays(const uint8_t *a, const uint8_t *b, size_t size)
{
#if 1
    bool result = false;
    for (size_t i = 0; (i < size) && (a[i] == b[i]); i++) {
        if (i == (size - 1)) {
            result = true;
        }
    }
#else
    bool result = true;
    for (size_t i = 0; i < size; i++) {
        if (a[i] != b[i]) {
            result = false;
            break;
        }
    }
#endif
    
    return result;
}

bool _is_invalid_block(const block block)
{
    return ((BLOCK_RESERVED_0 == block) || (BLOCK_RESERVED_1 == block) ||
            (BLOCK_RESERVED_2 == block) || (BLOCK_RESERVED_3 == block) ||
            (BLOCK_RESERVED_4 == block) || (BLOCK_RESERVED_5 == block) ||
            (BLOCK_RESERVED_6 == block) || (BLOCK_INVALID <= block));
}

/* [] END OF FILE */
