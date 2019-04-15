#ifndef `$INSTANCE_NAME`_UTILS_H_
#define `$INSTANCE_NAME`_UTILS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
    
#include "`$INSTANCE_NAME`_DEFS.h"

#define MAC_ADDR_SIZE       17
#define MAC_ADDR_LEN        MAC_ADDR_SIZE + 1
#define IP_ADDR_SIZE        15
#define IP_ADDR_LEN         IP_ADDR_SIZE + 1
#define SUBMASK_ADDR_SIZE   15
#define SUBMASK_ADDR_LEN    SUBMASK_ADDR_SIZE + 1
#define GATEWAY_ADDR_SIZE   15
#define GATEWAY_ADDR_LEN    GATEWAY_ADDR_SIZE + 1

#define MAC_INIT(a, b, c, d, e, f) \
{\
    .octect[0] = (uint8_t) a, \
    .octect[1] = (uint8_t) b, \
    .octect[2] = (uint8_t) c, \
    .octect[3] = (uint8_t) d, \
    .octect[4] = (uint8_t) e, \
    .octect[5] = (uint8_t) f  \
}\

#define IP_INIT(a, b, c, d) \
{\
    .octect[0] = (uint8_t) a,    \
    .octect[1] = (uint8_t) b,    \
    .octect[2] = (uint8_t) c,    \
    .octect[3] = (uint8_t) d,    \
}\

#define SUBMASK_INIT(a, b, c, d) \
{\
    .octect[0] = (uint8_t) a,    \
    .octect[1] = (uint8_t) b,    \
    .octect[2] = (uint8_t) c,    \
    .octect[3] = (uint8_t) d,    \
}\

#define GATEWAY_INIT(a, b, c, d) \
{\
    .octect[0] = (uint8_t) a,    \
    .octect[1] = (uint8_t) b,    \
    .octect[2] = (uint8_t) c,    \
    .octect[3] = (uint8_t) d,    \
}\

struct mac_addr {
    uint8_t octect[6];
};

struct ip_addr {
    uint8_t octect[4];
};

struct submask_addr {
    uint8_t octect[4];
};

struct gateway_addr {
    uint8_t octect[4];
};

struct wiz_info_t {
    struct mac_addr     mac;
    struct ip_addr      ip;
    struct submask_addr submask;
    struct gateway_addr gateway;
};

struct skt_addr_t {
    struct ip_addr  addr;
    uint16_t        port;
};

bool `$INSTANCE_NAME`_mac_from_string(struct mac_addr *mac, char *mac_str);
bool `$INSTANCE_NAME`_mac_to_string(struct mac_addr *mac,
    char *mac_str, size_t mac_len);
bool `$INSTANCE_NAME`_ip_from_string(struct ip_addr *ip, char *ip_str);
bool `$INSTANCE_NAME`_ip_to_string(struct ip_addr *ip,
    char *ip_str, size_t ip_len);
bool `$INSTANCE_NAME`_submask_from_string(struct submask_addr *submask,
    char *submask_str);
bool `$INSTANCE_NAME`_submask_to_string(struct submask_addr *submask,
    char *submask_str, size_t submask_len);
bool `$INSTANCE_NAME`_gateway_from_string(struct gateway_addr *gateway,
    char *gateway_str);
bool `$INSTANCE_NAME`_gateway_to_string(struct gateway_addr *gateway,
    char *gateway_str, size_t gateway_len);


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
