#if !defined(W5500_H_)
#define W5500_H_

#include <stdint.h>
#include <stddef.h>

void wiz_soft_reset(void);
void wiz_hard_reset(void);

void W5500_write_reg(uint16_t reg_addr, uint8_t *data, size_t data_size);
void W5500_write_reg_byte(uint16_t reg_addr, uint8_t data);

#endif

/* [] END OF FILE */
