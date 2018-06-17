#if !defined(COMMON_H_)
#define COMMON_H_

/* commonly used headers */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// MACRO functions to calculate the socket region block selects from the socket
// number.  These are helpful when working with the socket based commands.
#define ETH_SOCKET_BASE(s)  (((s * 4) + 1) << 3)
#define ETH_TX_BASE(s)      (((s * 4) + 2) << 3)
#define ETH_RX_BASE(s)      (((s * 4) + 3) << 3)

/* Sockets info */
/* i personally prefer enums over defines */
typedef enum {
    SOCKET_0,
    SOCKET_1,
    SOCKET_2,
    SOCKET_3,
    SOCKET_4,
    SOCKET_5,
    SOCKET_6,
    SOCKET_7,
    SOCKET_INVALID
} socket_n;

typedef enum {
    SOCKET_MODE_CLOSED  = 0x00,
    SOCKET_MODE_TCP     = 0x01,
    SOCKET_MODE_UDP     = 0x02,
    SOCKET_MODE_MACRAW  = 0x04,
} socket_mode;

typedef enum {
    SOCKET_STS_CLOSED       = 0,
    SOCKET_STS_INIT         = 0x13,
    SOCKET_STS_LISTEN       = 0x14,
    SOCKET_STS_SYNSENT      = 0x15,
    SOCKET_STS_SYNRECV      = 0x13,
    SOCKET_STS_ESTABLISHED  = 0x17,
    SOCKET_STS_FIN_WAIT     = 0x18,
    SOCKET_STS_CLOSING      = 0x1A,
    SOCKET_STS_TIME_WAIT    = 0x1B,
    SOCKET_STS_CLOSE_WAIT   = 0x1C,
    SOCKET_STS_LAST_ACK     = 0x1D,
    SOCKET_STS_UDP          = 0x22,
    SOCKET_STS_MACRAW       = 0x42,
} socket_sts;

typedef enum {
    SOCKET_CMD_OPEN         = 0x01,
    SOCKET_CMD_LISTEN       = 0x02,
    SOCKET_CMD_CONNECT      = 0x04,
    SOCKET_CMD_DISCONNECT   = 0x08,
    SOCKET_CMD_CLOSE        = 0x10,
    SOCKET_CMD_SEND         = 0x20,
    SOCKET_CMD_SEND_MAC     = 0x21,
    SOCKET_CMD_SEND_KEEP    = 0x22,
    SOCKET_CMD_RECV         = 0x40,
} socket_cmd;

typedef enum {
    SOCKET_REG_MODE                 = 0x0000,
    SOCKET_REG_COMMAND              = 0x0001,
    SOCKET_REG_INTERRUPT            = 0x0002,
    SOCKET_REG_STATUS               = 0x0003,
    SOCKET_REG_SOURCE_PORT0         = 0x0004,
    SOCKET_REG_SOURCE_PORT1         = 0x0005,
    SOCKET_REG_DESTINATION_HW_ADDR0 = 0x0006,
    SOCKET_REG_DESTINATION_HW_ADDR1 = 0x0007,
    SOCKET_REG_DESTINATION_HW_ADDR2 = 0x0008,
    SOCKET_REG_DESTINATION_HW_ADDR3 = 0x0009,
    SOCKET_REG_DESTINATION_HW_ADDR4 = 0x000A,
    SOCKET_REG_DESTINATION_HW_ADDR5 = 0x000B,
    SOCKET_REG_DESTINATION_IP_ADDR0 = 0x000C,
    SOCKET_REG_DESTINATION_IP_ADDR1 = 0x000D,
    SOCKET_REG_DESTINATION_IP_ADDR2 = 0x000E,
    SOCKET_REG_DESTINATION_IP_ADDR3 = 0x000F,
    SOCKET_REG_DESTINATION_PORT0    = 0x0010,
    SOCKET_REG_DESTINATION_PORT1    = 0x0011,
    SOCKET_REG_MAX_SEGMENT_SIZE0    = 0x0012,
    SOCKET_REG_MAX_SEGMENT_SIZE1    = 0x0013,
    SOCKET_REG_IP_TOS               = 0x0015,
    SOCKET_REG_IP_TTL               = 0x0016,
    SOCKET_REG_RX_BUFFER_SIZE       = 0x001E,
    SOCKET_REG_TX_BUFFER_SIZE       = 0x001F,
    SOCKET_REG_TX_FREE_SIZE0        = 0x0020,
    SOCKET_REG_TX_FREE_SIZE1        = 0x0021,
    SOCKET_REG_TX_READ_POINTER0     = 0x0022,
    SOCKET_REG_TX_READ_POINTER1     = 0x0023,
    SOCKET_REG_TX_WRITE_POINTER0    = 0x0024,
    SOCKET_REG_TX_WRITE_POINTER1    = 0x0025,
    SOCKET_REG_RX_RECEIVED_SIZE0    = 0x0026,
    SOCKET_REG_RX_RECEIVED_SIZE1    = 0x0027,
    SOCKET_REG_RX_READ_POINTER0     = 0x0028,
    SOCKET_REG_RX_READ_POINTER1     = 0x0029,
    SOCKET_REG_RX_WRITE_POINTER0    = 0x002A,
    SOCKET_REG_RX_WRITE_POINTER1    = 0x002B,
    SOCKET_REG_INTERRUPT_MASK       = 0x002C,
    SOCKET_REG_FRAGMENT_OFFSET_INIP0= 0x002D,
    SOCKET_REG_FRAGMENT_OFFSET_INIP1= 0x002E,
    SOCKET_REG_KEEP_ALIVE_TIMER     = 0x002F
} socket_reg;

/* Remove unnecesary enums, use base address + offset */
typedef enum {
    BLOCK_COMMON_REGISTER   = 0,
    BLOCK_SOCKET_0_REGISTER,
    BLOCK_SOCKET_0_TX_BUFFER,
    BLOCK_SOCKET_0_RX_BUFFER,
    BLOCK_RESERVED_0,
    BLOCK_SOCKET_1_REGISTER,
    BLOCK_SOCKET_1_TX_BUFFER,
    BLOCK_SOCKET_1_RX_BUFFER,
    BLOCK_RESERVED_1,
    BLOCK_SOCKET_2_REGISTER,
    BLOCK_SOCKET_2_TX_BUFFER,
    BLOCK_SOCKET_2_RX_BUFFER,
    BLOCK_RESERVED_2,
    BLOCK_SOCKET_3_REGISTER,
    BLOCK_SOCKET_3_TX_BUFFER,
    BLOCK_SOCKET_3_RX_BUFFER,
    BLOCK_RESERVED_3,
    BLOCK_SOCKET_4_REGISTER,
    BLOCK_SOCKET_4_TX_BUFFER,
    BLOCK_SOCKET_4_RX_BUFFER,
    BLOCK_RESERVED_4,
    BLOCK_SOCKET_5_REGISTER,
    BLOCK_SOCKET_5_TX_BUFFER,
    BLOCK_SOCKET_5_RX_BUFFER,
    BLOCK_RESERVED_5,
    BLOCK_SOCKET_6_REGISTER,
    BLOCK_SOCKET_6_TX_BUFFER,
    BLOCK_SOCKET_6_RX_BUFFER,
    BLOCK_RESERVED_6,
    BLOCK_SOCKET_7_REGISTER,
    BLOCK_SOCKET_7_TX_BUFFER,
    BLOCK_SOCKET_7_RX_BUFFER,
    BLOCK_INVALID
} block;

typedef enum {
    ACCESS_MODE_READ    = 0,
    ACCESS_MODE_WRITE,
    ACCESS_MODE_INVALID
} access_mode;

typedef enum {
    SPI_OPERATION_MODE_VDM   = 0,
    SPI_OPERATION_MODE_FDM_1BYTE,
    SPI_OPERATION_MODE_FDM_2BYTE,
    SPI_OPERATION_MODE_FDM_4BYTE,
    SPI_OPERATION_MODE_INVALID
} spi_operation_mode;

/* module representation */
#define OPAQUE_STRUCTS  0
#if (OPAQUE_STRUCTS == 0)
struct socket {
    socket_sts  status;
    socket_mode protocol;
    uint16_t    port;
};

#define W5500_SOCKETS   8
struct W5500 {
    uint8_t         mac[6];
    uint8_t         ip[4];
    uint8_t         gateway[4];
    uint8_t         subnet[4];
    struct socket   socket[W5500_SOCKETS];
};
#else
struct socket;
struct W5500;
#endif
extern struct W5500 this;

bool _is_invalid_block(const block block);
bool compare_arrays(const uint8_t *a, const uint8_t *b, size_t size);

#endif /* common_h_ */

/* [] END OF FILE */
