#include "project.h"

#include "hal_spi.h"
#include "common.h"

#if 0
// spi2 hooks
static void spi_hook_write_byte(const uint8_t *addr_ctrl_phase, const size_t addr_ctrl_phase_size,
                                const uint8_t data)
{
    SPI_hook_ClearFIFO();
    
    SS_hook_Write(0);
    
    for (size_t i = 0; i < addr_ctrl_phase_size; i++) {
        SPI_hook_WriteTxData(addr_ctrl_phase[i]);
        while (!(SPI_hook_ReadTxStatus() & SPI_hook_STS_BYTE_COMPLETE)) {
        }
    }
    SPI_hook_ClearFIFO();

    SPI_hook_WriteTxData(data);
    while (!(SPI_hook_ReadTxStatus() & SPI_hook_STS_BYTE_COMPLETE)) {
    }
    
    SS_hook_Write(1);
}
#endif

enum {
    _SPI_MODE_POS       = 0,
    _ACCESS_MODE_POS    = 2,
    _BLOCK_POS          = 3,
};

void W5500_spi_read(const uint8_t block, const uint16_t reg_addr, uint8_t *data, const size_t data_size)
{
    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (ACCESS_MODE_READ << _ACCESS_MODE_POS) |
                                  (SPI_OPERATION_MODE_VDM << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(reg_addr), LO8(reg_addr), control_phase
    };

    SPI_ClearFIFO();
    
    // This can go to the SPI Hook ;)
    // So we will pass the addr_ctrl_phase, this is always 3 bytes
    // and then the data, which is data_size in VLA mode
    // spi_hook(uint8_t addr_ctrl_phase, addr_ctrl_phase_size, data, data_size)
    
    SS_Write(0);
    
    for (size_t i = 0; i < sizeof(addr_ctrl_phase); i++) {
        SPI_WriteTxData(addr_ctrl_phase[i]);
        while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
        }
    }
    SPI_ClearFIFO();

    for (size_t i = 0; i < data_size; i++) {
        SPI_WriteTxData(0x00);
        while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
        }
        data[i] = SPI_ReadRxData();
    }
    
    SS_Write(1);
}

void W5500_spi_write(const uint8_t block, const uint16_t reg_addr, const uint8_t *data, const size_t data_size)
{
    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (ACCESS_MODE_WRITE << _ACCESS_MODE_POS) |
                                  (SPI_OPERATION_MODE_VDM << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(reg_addr), LO8(reg_addr), control_phase
    };

    SPI_ClearFIFO();
    
    SS_Write(0);
    
    for (size_t i = 0; i < sizeof(addr_ctrl_phase); i++) {
        SPI_WriteTxData(addr_ctrl_phase[i]);
        while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
        }
    }
    SPI_ClearFIFO();

    for (size_t i = 0; i < data_size; i++) {
        SPI_WriteTxData(data[i]);
        while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
        }
    }
    
    SS_Write(1);
}

uint8_t W5500_spi_read_byte(const uint8_t block, const uint16_t reg_addr)
{
    uint8_t result = 0;
    
    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (ACCESS_MODE_READ << _ACCESS_MODE_POS) |
                                  (SPI_OPERATION_MODE_VDM << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(reg_addr), LO8(reg_addr), control_phase
    };

    SPI_ClearFIFO();
    
    SS_Write(0);
    
    for (size_t i = 0; i < sizeof(addr_ctrl_phase); i++) {
        SPI_WriteTxData(addr_ctrl_phase[i]);
        while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
        }
    }
    SPI_ClearFIFO();

    SPI_WriteTxData(0x00);
    while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
    }
    result = SPI_ReadRxData();
    
    SS_Write(1);
    
    return result;
}

void W5500_spi_write_byte(const uint8_t block, const uint16_t reg_addr, uint8_t data)
{
    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (ACCESS_MODE_WRITE << _ACCESS_MODE_POS) |
                                  (SPI_OPERATION_MODE_VDM << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(reg_addr), LO8(reg_addr), control_phase
    };

    SPI_ClearFIFO();
    
    SS_Write(0);
    
    for (size_t i = 0; i < sizeof(addr_ctrl_phase); i++) {
        SPI_WriteTxData(addr_ctrl_phase[i]);
        while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
        }
    }
    SPI_ClearFIFO();

    SPI_WriteTxData(data);
    while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
    }
    
    SS_Write(1);
}

/* [] END OF FILE */
