#if !defined(HAL_SPI_H_)
#define HAL_SPI_H_

#include <stddef.h>
#include <stdint.h>

void W5500_spi_read(const uint8_t block, const uint16_t reg_addr, uint8_t *data, const size_t data_size);
void W5500_spi_write(const uint8_t block, const uint16_t reg_addr, const uint8_t *data, const size_t data_size);
uint8_t W5500_spi_read_byte(const uint8_t block, const uint16_t reg_addr);
void W5500_spi_write_byte(const uint8_t block, const uint16_t reg_addr, uint8_t data);

#endif /* hal_spi_h_ */

/* [] END OF FILE */
