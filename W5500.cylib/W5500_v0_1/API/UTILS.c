#include <string.h>
#include <stdio.h>

#include "`$INSTANCE_NAME`_UTILS.h"

bool `$INSTANCE_NAME`_mac_from_string(struct mac_addr *mac, char *mac_str)
{
    mac->octect[0] = 0;
    mac->octect[1] = 0;
    mac->octect[2] = 0;
    mac->octect[3] = 0;
    mac->octect[4] = 0;
    mac->octect[5] = 0;
    
    return false;
}

bool `$INSTANCE_NAME`_mac_to_string(struct mac_addr *mac,
    char *mac_str, size_t mac_len)
{
    snprintf(mac_str, mac_len, "%02X-%02X-%02X-%02X-%02X-%02X",
             mac->octect[0], mac->octect[1], mac->octect[2],
             mac->octect[3], mac->octect[4], mac->octect[5]);
    
    return false;
}

bool `$INSTANCE_NAME`_ip_from_string(struct ip_addr *ip, char *ip_str)
{
    return false;
}

bool `$INSTANCE_NAME`_ip_to_string(struct ip_addr *ip,
    char *ip_str, size_t ip_len)
{
    snprintf(ip_str, ip_len, "%d.%d.%d.%d",
             ip->octect[0], ip->octect[1], ip->octect[2], ip->octect[3]);
    
    return false;
}

bool `$INSTANCE_NAME`_submask_from_string(struct submask_addr *submask,
    char *submask_str)
{
    return false;
}

bool `$INSTANCE_NAME`_submask_to_string(struct submask_addr *submask,
    char *submask_str, size_t submask_len)
{
    return false;
}

bool `$INSTANCE_NAME`_gateway_from_string(struct gateway_addr *gateway,
    char *gateway_str)
{
    return false;
}

bool `$INSTANCE_NAME`_gateway_to_string(struct gateway_addr *gateway,
    char *gateway_str, size_t gateway_len)
{
    return false;
}

/* [] END OF FILE */
