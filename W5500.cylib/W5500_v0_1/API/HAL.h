/**
* @file     `$INSTANCE_NAME`_HAL.h
* @version  1
*/

#ifndef `$INSTANCE_NAME`_HAL_H
#define `$INSTANCE_NAME`_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "`$INSTANCE_NAME`_DEFS.h"

/* SPI Control */

// Function pointer to send and receive data from the W5500 chip
typedef void (*nrf_spi_xfer)(const uint8_t *in, uint8_t *out, const size_t xfer_size);

extern nrf_spi_xfer `$INSTANCE_NAME`_spi_xfer;

// Default function, spi sends byte by byte
void `$INSTANCE_NAME`_default_spi_xfer(const uint8_t *in, uint8_t *out, const size_t xfer_size);
// Register your own spi xfer function
void `$INSTANCE_NAME`_register_spi_xfer(nrf_spi_xfer new_spi_xfer);

void `$INSTANCE_NAME`_spi_read(const uint8_t block, const uint16_t reg_addr, uint8_t *data, const size_t data_size);
void `$INSTANCE_NAME`_spi_write(const uint8_t block, const uint16_t reg_addr, const uint8_t *data, const size_t data_size);
uint8_t `$INSTANCE_NAME`_spi_read_byte(const uint8_t block, const uint16_t reg_addr);
void `$INSTANCE_NAME`_spi_write_byte(const uint8_t block, const uint16_t reg_addr, uint8_t data);

typedef enum {
    WIZ_RST_HARDWARE,
    WIZ_RST_SOFTWARE
} wiz_rst;

void `$INSTANCE_NAME`_reset(wiz_rst rst);
void `$INSTANCE_NAME`_write_reg(uint16_t reg_addr, uint8_t *data, size_t data_size);
void `$INSTANCE_NAME`_write_reg_byte(uint16_t reg_addr, uint8_t data);

/*
bool `$INSTANCE_NAME`_read_bit(const nrf_register reg, const uint8_t bit_pos);
void `$INSTANCE_NAME`_clear_bit(const nrf_register reg, const uint8_t bit_pos);
void `$INSTANCE_NAME`_set_bit(const nrf_register reg, const uint8_t bit_pos);
*/

/* IO Control */
typedef enum {
    GPIO_RESET,
    GPIO_SET
} nrf_gpio;

void `$INSTANCE_NAME`_ss_write(nrf_gpio state);
void `$INSTANCE_NAME`_rst_write(nrf_gpio state);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* `$INSTANCE_NAME`_HAL_H */

/* [] END OF FILE */
