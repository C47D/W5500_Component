#if !defined(`$INSTANCE_NAME`_UTILS_H_)
#define `$INSTANCE_NAME`_UTILS_H_

#include <stdint.h>
#include <stdbool.h>
    
#include "`$INSTANCE_NAME`_DEFS.h"

#define MAC_INIT(a, b, c, d, e, f) \
{\
    ._addr[0] = (uint8_t) a, \
    ._addr[1] = (uint8_t) b, \
    ._addr[2] = (uint8_t) c, \
    ._addr[3] = (uint8_t) d, \
    ._addr[4] = (uint8_t) e, \
    ._addr[5] = (uint8_t) f  \
}\

#define IP_INIT(a, b, c, d) \
{\
    ._addr[0] = (uint8_t) a,    \
    ._addr[1] = (uint8_t) b,    \
    ._addr[2] = (uint8_t) c,    \
    ._addr[3] = (uint8_t) d,    \
}\

#define SUBMASK_INIT(a, b, c, d) \
{\
    ._addr[0] = (uint8_t) a,    \
    ._addr[1] = (uint8_t) b,    \
    ._addr[2] = (uint8_t) c,    \
    ._addr[3] = (uint8_t) d,    \
}\

#define GATEWAY_INIT(a, b, c, d) \
{\
    ._addr[0] = (uint8_t) a,    \
    ._addr[1] = (uint8_t) b,    \
    ._addr[2] = (uint8_t) c,    \
    ._addr[3] = (uint8_t) d,    \
}\

struct mac_addr {
    uint8_t _addr[6];
};

struct ip_addr {
    uint8_t _addr[4];
};

struct submask_addr {
    uint8_t _addr[4];
};

struct gateway_addr {
    uint8_t _addr[4];
};

struct wiz_info_t {
    struct mac_addr     mac_addr;
    struct ip_addr      ip_addr;
    struct submask_addr submask_addr;
    struct gateway_addr gateway_addr;
};

struct skt_addr_t {
    struct ip_addr  addr;
    uint16_t        port;
};

void mac_from_string(struct mac_addr *mac, char *mac_str);
void mac_to_string(struct mac_addr *mac, char *mac_str);
void ip_from_string(struct ip_addr *ip, char *mac_str);
void ip_to_string(struct ip_addr *ip, char *mac_str);
/*
bool _is_invalid_block(const block block)
{
    return ((BLOCK_RESERVED_0 == block) || (BLOCK_RESERVED_1 == block) ||
            (BLOCK_RESERVED_2 == block) || (BLOCK_RESERVED_3 == block) ||
            (BLOCK_RESERVED_4 == block) || (BLOCK_RESERVED_5 == block) ||
            (BLOCK_RESERVED_6 == block) || (BLOCK_INVALID <= block));
}

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
*/
#endif /* utils_h_ */

/* [] END OF FILE */
