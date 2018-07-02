#include "project.h"

#include "W5500.h"
#include "common.h"
#include "hal_spi.h"

void wiz_soft_reset(void)
{
    /* Reset the W5500, this can be done with a command or the /RST pin,
     * the component will handle this depending if the user provide a /RST pin. */
    W5500_spi_write_byte(BLOCK_COMMON_REGISTER, SOCKET_REG_MODE, 0x80);
}

void wiz_hard_reset(void)
{
    RST_Write(0);
    CyDelay(50);
    RST_Write(1);
}

void W5500_write_reg(uint16_t reg_addr, uint8_t *data, size_t data_size)
{
    W5500_spi_write(BLOCK_COMMON_REGISTER, reg_addr, data, data_size);
}

void W5500_write_reg_byte(uint16_t reg_addr, uint8_t data)
{
    W5500_spi_write_byte(BLOCK_COMMON_REGISTER, reg_addr, data);
}

/* [] END OF FILE */
