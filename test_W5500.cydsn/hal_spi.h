#if !defined(HAL_SPI_H_)
#define HAL_SPI_H_

#include <stddef.h>
#include <stdint.h>

#include "common.h"

void W5500_spi_read(const block block, const uint16_t address, uint8_t *data, const size_t data_size);
void W5500_spi_write(const block block, const uint16_t address, uint8_t *data, const size_t data_size);
uint8_t W5500_spi_read_byte(const block block, const uint16_t address);
void W5500_spi_write_byte(const block block, const uint16_t address, uint8_t data);

#endif /* hal_spi_h_ */

/* [] END OF FILE */
