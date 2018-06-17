#include "project.h"

#include "hal_spi.h"
#include "common.h"

enum {
    _SPI_MODE_POS       = 0,
    _ACCESS_MODE_POS    = 2,
    _BLOCK_POS          = 3,
};

static void W5500_spi(const block block, const uint16_t address,
                      const access_mode access_mode, const spi_operation_mode spi_mode,
                      uint8_t *data, const size_t data_size)
{
    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (access_mode << _ACCESS_MODE_POS) |
                                  (spi_mode << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(address), LO8(address), control_phase
    };

    SPI_ClearFIFO();
    
    SS_Write(0);
    
    for (size_t i = 0; i < sizeof(addr_ctrl_phase); i++) {
        SPI_WriteTxData(addr_ctrl_phase[i]);
        while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
        }
    }
    SPI_ClearFIFO();

    if (ACCESS_MODE_READ == access_mode) {
        for (size_t i = 0; i < data_size; i++) {
            SPI_WriteTxData(0xFF);
            while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
            }
            data[i] = SPI_ReadRxData();
        }
    } else { /* ACCESS_MODE_WRITE */
        for (size_t i = 0; i < data_size; i++) {
            SPI_WriteTxData(data[i]);
            while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
            }
        }
    }
    
    SS_Write(1);
}

void W5500_spi_read(const block block, const uint16_t address,
                           uint8_t *data, const size_t data_size)
{
    if ((NULL == data) || (0 == data_size)) {
        return;
    }
    
    if (_is_invalid_block(block)) {
        return;
    }
#if 1
    W5500_spi(block, address, ACCESS_MODE_READ, SPI_OPERATION_MODE_VDM, data, data_size);
#else
    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (ACCESS_MODE_READ << _ACCESS_MODE_POS) |
                                  (SPI_OPERATION_MODE_VDM << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(address), LO8(address), control_phase
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
        SPI_WriteTxData(0xFF);
        while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
        }
        data[i] = SPI_ReadRxData();
    }
    
    SS_Write(1);
#endif
}

void W5500_spi_write(const block block, const uint16_t address,
                     uint8_t *data, const size_t data_size)
{
    if ((NULL == data) || (0 == data_size)) {
        return;
    }
    
    if (_is_invalid_block(block)) {
        return;
    }
#if 1
    W5500_spi(block, address, ACCESS_MODE_WRITE, SPI_OPERATION_MODE_VDM, data, data_size);
#else
    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (ACCESS_MODE_WRITE << _ACCESS_MODE_POS) |
                                  (SPI_OPERATION_MODE_VDM << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(address), LO8(address), control_phase
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
#endif
}

uint8_t W5500_spi_read_byte(const block block, const uint16_t address)
{
    uint8_t result = 0;
    
    if (_is_invalid_block(block)) {
        return 0;
    }
    
    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (ACCESS_MODE_READ << _ACCESS_MODE_POS) |
                                  (SPI_OPERATION_MODE_VDM << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(address), LO8(address), control_phase
    };

    SPI_ClearFIFO();
    
    SS_Write(0);
    
    for (size_t i = 0; i < sizeof(addr_ctrl_phase); i++) {
        SPI_WriteTxData(addr_ctrl_phase[i]);
        while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
        }
    }
    SPI_ClearFIFO();

    SPI_WriteTxData(0xFF);
    while (!(SPI_ReadTxStatus() & SPI_STS_BYTE_COMPLETE)) {
    }
    result = SPI_ReadRxData();
    
    SS_Write(1);
    
    return result;
}

void W5500_spi_write_byte(const block block, const uint16_t address, uint8_t data)
{
    if (_is_invalid_block(block)) {
        return;
    }

    const uint8_t control_phase = (block << _BLOCK_POS) |
                                  (ACCESS_MODE_WRITE << _ACCESS_MODE_POS) |
                                  (SPI_OPERATION_MODE_VDM << _SPI_MODE_POS);

    const uint8_t addr_ctrl_phase[] = {
        HI8(address), LO8(address), control_phase
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
