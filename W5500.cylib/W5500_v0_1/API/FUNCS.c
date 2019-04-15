#include "project.h"

#include <stdbool.h>

#include "`$INSTANCE_NAME`_FUNCS.h"
#include "`$INSTANCE_NAME`_HAL.h"
#include "`$INSTANCE_NAME`_UTILS.h"

#define W5500_STARTUP_DELAY 100

static bool `$INSTANCE_NAME`_initialized = false;

static struct wiz_info_t `$INSTANCE_NAME`_info = {
    .mac = {0},
    .ip = {0},
    .submask = {0},
    .gateway = {0}
};

void `$INSTANCE_NAME`_start(void)
{
    `$INSTANCE_NAME`_reset(WIZ_RST_SOFTWARE);
    
    `$INSTANCE_NAME`_mac_from_string(&`$INSTANCE_NAME`_info.mac,
        `$INSTANCE_NAME`_MAC_ADDR);
    `$INSTANCE_NAME`_ip_from_string(&`$INSTANCE_NAME`_info.ip,
        `$INSTANCE_NAME`_IP_ADDR);
    `$INSTANCE_NAME`_submask_from_string(&`$INSTANCE_NAME`_info.submask,
        `$INSTANCE_NAME`_SUBMASK_ADDR);
    `$INSTANCE_NAME`_gateway_from_string(&`$INSTANCE_NAME`_info.gateway,
        `$INSTANCE_NAME`_GATEWAY_ADDR);
    
    `$INSTANCE_NAME`_init(&`$INSTANCE_NAME`_info);
}

void `$INSTANCE_NAME`_init(const struct wiz_info_t *wiz_info)
{
    `$INSTANCE_NAME`_initialized = true;
    
    // TODO: Read status register
    
    /* Allow W5500 to start-up */
    CyDelay(W5500_STARTUP_DELAY);
    
    // SHAR (Source Hardware Address Register)
    `$INSTANCE_NAME`_spi_write(BLOCK_COMMON_REGISTER, 0x0009, wiz_info->mac.octect,
        sizeof(wiz_info->mac.octect));
    // gateway address register
    `$INSTANCE_NAME`_spi_write(BLOCK_COMMON_REGISTER, 0x0001, wiz_info->gateway.octect,
        sizeof(wiz_info->gateway.octect));
    // submask address
    `$INSTANCE_NAME`_spi_write(BLOCK_COMMON_REGISTER, 0x0005, wiz_info->submask.octect,
        sizeof(wiz_info->submask.octect));
    // ip address
    `$INSTANCE_NAME`_spi_write(BLOCK_COMMON_REGISTER, 0x000F, wiz_info->ip.octect,
        sizeof(wiz_info->ip.octect));
}

/* [] END OF FILE */
