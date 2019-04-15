/**
* @file     `$INSTANCE_NAME`_HAL.c
* @version  3
* @brief    The nRF24 radio is controlled via SPI, this file have all the SPI
* communication between the PSoC and the nRF24 radio.
*/

#include <stdint.h>
#include <string.h>

#include "`$INSTANCE_NAME`_CONFIG.h"
#include "`$INSTANCE_NAME`_HAL.h"
#include "`$INSTANCE_NAME`_DEFS.h"

#if defined (_PSOC6)
# include "gpio/cy_gpio.h"
#else // (_PSOC_UDB) || (_PSOC4)
# if defined (_PSOC4_SCB)
#  include "`$SPI_MASTER`_SPI_UART.h" // may be no longer necessary
# endif
# include "`$SS_PIN`.h"
# if (`$INSTANCE_NAME`_HW_RST == 1)
# include "`$RST_PIN`.h"
# endif
#endif

static void `$INSTANCE_NAME`_spi_clear_fifo(void);

spi_xfer `$INSTANCE_NAME`_spi_xfer = `$INSTANCE_NAME`_default_spi_xfer;

void `$INSTANCE_NAME`_register_spi_xfer(spi_xfer new_spi_xfer)
{
    `$INSTANCE_NAME`_spi_xfer = new_spi_xfer;
}

static void `$INSTANCE_NAME`_soft_reset(void);
static void `$INSTANCE_NAME`_hard_reset(void);

// FIXME: Only use hardware reset if the reset pin is being used.
void `$INSTANCE_NAME`_reset(wiz_rst rst)
{
    if (WIZ_RST_SOFTWARE == rst) {
        `$INSTANCE_NAME`_soft_reset();
    } else if (WIZ_RST_HARDWARE == rst){
        `$INSTANCE_NAME`_hard_reset(); 
    }
}

void `$INSTANCE_NAME`_write_reg(uint16_t reg_addr, uint8_t *data, size_t data_size)
{
    `$INSTANCE_NAME`_spi_write(BLOCK_COMMON_REGISTER, reg_addr, data, data_size);
}

void `$INSTANCE_NAME`_write_reg_byte(uint16_t reg_addr, uint8_t data)
{
    `$INSTANCE_NAME`_spi_write_byte(BLOCK_COMMON_REGISTER, reg_addr, data);
}

enum {
    _SPI_MODE_POS       = 0,
    _ACCESS_MODE_POS    = 2,
    _BLOCK_POS          = 3,
};

void `$INSTANCE_NAME`_spi_read(const uint8_t block, const uint16_t reg_addr, uint8_t *data, const size_t data_size)
{
    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (ACCESS_MODE_READ << _ACCESS_MODE_POS) |
                                  (SPI_OPERATION_MODE_VDM << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(reg_addr), LO8(reg_addr), control_phase
    };

    `$INSTANCE_NAME`_spi_clear_fifo();
    
    `$INSTANCE_NAME`_ss_write(GPIO_RESET);

#if defined (_PSOC6)
    
#elif defined (_PSOC4_SCB)

#else
    for (size_t i = 0; i < data_size; i++) {
        `$SPI_MASTER`_WriteTxData(addr_ctrl_phase[i]);
        while (!(`$SPI_MASTER`_ReadTxStatus() & `$SPI_MASTER`_STS_BYTE_COMPLETE)) {
        }
        (void) `$SPI_MASTER`_ReadRxData();
    }
    
    `$INSTANCE_NAME`_spi_clear_fifo();
    
    for (size_t i = 0; i < data_size; i++) {
        `$SPI_MASTER`_WriteTxData(0x00);
        while (!(`$SPI_MASTER`_ReadTxStatus() & `$SPI_MASTER`_STS_BYTE_COMPLETE)) {
        }
        data[i] = `$SPI_MASTER`_ReadRxData();
    }
#endif
    
    `$INSTANCE_NAME`_ss_write(GPIO_SET);
}

void `$INSTANCE_NAME`_spi_write(const uint8_t block, const uint16_t reg_addr, const uint8_t *data, const size_t data_size)
{
    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (ACCESS_MODE_WRITE << _ACCESS_MODE_POS) |
                                  (SPI_OPERATION_MODE_VDM << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(reg_addr), LO8(reg_addr), control_phase
    };

    `$INSTANCE_NAME`_spi_clear_fifo();
    
    `$INSTANCE_NAME`_ss_write(GPIO_RESET);

#if defined (_PSOC6)
    
#elif defined (_PSOC4_SCB)

#else
    for (size_t i = 0; i < data_size; i++) {
        `$SPI_MASTER`_WriteTxData(addr_ctrl_phase[i]);
        while (!(`$SPI_MASTER`_ReadTxStatus() & `$SPI_MASTER`_STS_BYTE_COMPLETE)) {
        }
        (void) `$SPI_MASTER`_ReadRxData();
    }
    
    `$INSTANCE_NAME`_spi_clear_fifo();
    
    for (size_t i = 0; i < data_size; i++) {
        `$SPI_MASTER`_WriteTxData(data[i]);
        while (!(`$SPI_MASTER`_ReadTxStatus() & `$SPI_MASTER`_STS_BYTE_COMPLETE)) {
        }
        (void) `$SPI_MASTER`_ReadRxData();
    }
#endif

    `$INSTANCE_NAME`_ss_write(GPIO_SET);
}

uint8_t `$INSTANCE_NAME`_spi_read_byte(const uint8_t block, const uint16_t reg_addr)
{
    uint8_t result = 0;
    
    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (ACCESS_MODE_READ << _ACCESS_MODE_POS) |
                                  (SPI_OPERATION_MODE_VDM << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(reg_addr), LO8(reg_addr), control_phase
    };

    `$INSTANCE_NAME`_spi_clear_fifo();
    
    `$INSTANCE_NAME`_ss_write(GPIO_RESET);
    
#if defined (_PSOC6)
    
#elif defined (_PSOC4_SCB)

#else    
    for (size_t i = 0; i < sizeof(addr_ctrl_phase); i++) {
        `$SPI_MASTER`_WriteTxData(addr_ctrl_phase[i]);
        while (!(`$SPI_MASTER`_ReadTxStatus() & `$SPI_MASTER`_STS_BYTE_COMPLETE)) {
        }
        (void) `$SPI_MASTER`_ReadRxData();
    }
    
    // THIS MIGHT BE UNNECESSARY
    `$INSTANCE_NAME`_spi_clear_fifo();

    `$SPI_MASTER`_WriteTxData(0x00);
    while (!(`$SPI_MASTER`_ReadTxStatus() & `$SPI_MASTER`_STS_BYTE_COMPLETE)) {
    }
    result = `$SPI_MASTER`_ReadRxData();
#endif

    `$INSTANCE_NAME`_ss_write(GPIO_SET);
    
    return result;
}

void `$INSTANCE_NAME`_spi_write_byte(const uint8_t block, const uint16_t reg_addr, uint8_t data)
{
    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (ACCESS_MODE_WRITE << _ACCESS_MODE_POS) |
                                  (SPI_OPERATION_MODE_VDM << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(reg_addr), LO8(reg_addr), control_phase
    };

    `$INSTANCE_NAME`_spi_clear_fifo();
    
    `$INSTANCE_NAME`_ss_write(GPIO_RESET);

#if defined (_PSOC6)
    
#elif defined (_PSOC4_SCB)

#else
    for (size_t i = 0; i < sizeof(addr_ctrl_phase); i++) {
        `$SPI_MASTER`_WriteTxData(addr_ctrl_phase[i]);
        while (!(`$SPI_MASTER`_ReadTxStatus() & `$SPI_MASTER`_STS_BYTE_COMPLETE)) {
        }
        (void) `$SPI_MASTER`_ReadRxData();
    }
    
    // THIS MIGHT BE UNNECESSARY
    `$INSTANCE_NAME`_spi_clear_fifo();

    `$SPI_MASTER`_WriteTxData(data);
    while (!(`$SPI_MASTER`_ReadTxStatus() & `$SPI_MASTER`_STS_BYTE_COMPLETE)) {
    }
    (void) `$SPI_MASTER`_ReadRxData();
#endif

    `$INSTANCE_NAME`_ss_write(GPIO_SET);
}

static void `$INSTANCE_NAME`_soft_reset(void)
{
    `$INSTANCE_NAME`_spi_write_byte(BLOCK_COMMON_REGISTER, SOCKET_REG_MODE, 0x80);
}

static void `$INSTANCE_NAME`_hard_reset(void)
{
    `$RST_PIN`_Write(0);
    CyDelay(50);
    `$RST_PIN`_Write(1);
}

static void `$INSTANCE_NAME`_spi_clear_fifo(void)
{
#if defined (_PSOC6)
    Cy_SCB_ClearRxFifo(`$SPI_MASTER`_HW);
    Cy_SCB_ClearTxFifo(`$SPI_MASTER`_HW);
#elif defined (_PSOC4_SCB)
    `$SPI_MASTER`_SpiUartClearRxBuffer();
    `$SPI_MASTER`_SpiUartClearTxBuffer();
#elif defined (_PSOC_UDB)
    `$SPI_MASTER`_ClearFIFO();
#else
    #error "Non valid PSoC device identified."
#endif
}

void `$INSTANCE_NAME`_ss_write(nrf_gpio state)
{
    if (GPIO_RESET == state) {
#if defined (_PSOC6)
    Cy_GPIO_Clr(`$SS_PIN`_PORT, `$SS_PIN`_NUM);
#else // _PSoC4_SCB | _PSOC_UDB
    `$SS_PIN`_Write(0);
#endif
    } else { // GPIO_SET
#if defined (_PSOC6)
    Cy_GPIO_Set(`$SS_PIN`_PORT, `$SS_PIN`_NUM);
#else // _PSoC4_SCB | _PSOC_UDB
    `$SS_PIN`_Write(1);
#endif
    }
}

void `$INSTANCE_NAME`_rst_write(nrf_gpio state)
{
    if (GPIO_RESET == state) {
#if defined (_PSOC6)
    Cy_GPIO_Clr(`$RST_PIN`_PORT, `$RST_PIN`_NUM);
#else // _PSoC4_SCB | _PSOC_UDB
    `$RST_PIN`_Write(0);
#endif
    } else { // GPIO_SET
#if defined (_PSOC6)
    Cy_GPIO_Set(`$RST_PIN`_PORT, `$RST_PIN`_NUM);
#else // _PSoC4_SCB | _PSOC_UDB
    `$RST_PIN`_Write(1);
#endif
    }
}

/* [] END OF FILE */
