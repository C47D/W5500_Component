#include "project.h"

#include "`$INSTANCE_NAME`_FUNCS.h"
#include "`$INSTANCE_NAME`_HAL.h"
#include "`$INSTANCE_NAME`_UTILS.h"

void wiz_init(const struct wiz_info_t *wiz_info)
{
    /* Allow W5500 to start-up */
    // TODO: Read status register
    CyDelay(100);
    
    // SHAR (Source Hardware Address Register)
    `$INSTANCE_NAME`_spi_write(BLOCK_COMMON_REGISTER, 0x0009, wiz_info->mac_addr._addr,
        sizeof(wiz_info->mac_addr._addr));
    // gateway address register
    `$INSTANCE_NAME`_spi_write(BLOCK_COMMON_REGISTER, 0x0001, wiz_info->gateway_addr._addr, 
        sizeof(wiz_info->gateway_addr._addr));
    // submask address
    `$INSTANCE_NAME`_spi_write(BLOCK_COMMON_REGISTER, 0x0005, wiz_info->submask_addr._addr,
        sizeof(wiz_info->submask_addr._addr));
    // ip address
    `$INSTANCE_NAME`_spi_write(BLOCK_COMMON_REGISTER, 0x000F, wiz_info->ip_addr._addr,
        sizeof(wiz_info->ip_addr._addr));
}

/* [] END OF FILE */
