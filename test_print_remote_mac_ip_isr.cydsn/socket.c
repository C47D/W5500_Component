/**
 * Socket memory map
 * offset   register
 * 0x0000   socket n mode (Sn_MR)
 * 0x0001   socket n command (Sn_CR)
 * 0x0002   socket n interrupt (Sn_IR)
 * 0x0003   socket n status (Sn_SR)
 * 0x0004   socket n source port 0 (Sn_PORT0)
 * 0x0005   socket n source port 1 (Sn_PORT1)
 * 0x0006   socket n destination hardware address (Sn_DHAR0)
 * 0x0007   socket n destination hardware address (Sn_DHAR1)
 * 0x0008   socket n destination hardware address (Sn_DHAR2)
 * 0x0009   socket n destination hardware address (Sn_DHAR3)
 * 0x000A   socket n destination hardware address (Sn_DHAR4)
 * 0x000B   socket n destination hardware address (Sn_DHAR5)
 * 0x000C   socket n destination ip address (Sn_DIPR0)
 * 0x000D   socket n destination ip address (Sn_DIPR1)
 * 0x000E   socket n destination ip address (Sn_DIPR2)
 * 0x000F   socket n destination ip address (Sn_DIPR3)
 * 0x0010   socket n destination port (Sn_DPORT0)
 * 0x0011   socket n destination port (Sn_DPORT1)
 * 0x0012   socket n maximum segment size (Sn_MSSR0)
 * 0x0013   socket n maximum segment size (Sn_MSSR1)
 * 0x0014   socket n reserved
 * 0x0015   socket n ip tos (Sn_TOS)
 * 0x0016   socket n ip ttl (Sn_TTL)
 * 0x0017 ... 0x001D reserved
 * 0x001E   socket n receive buffer size (Sn_RXBUF_SIZE)
 * 0x001F   socket n transmit buffer size (Sn_TXBUF_SIZE)
 * 0x0020   socket n tx free size (Sn_FSR0)
 * 0x0021   socket n tx free size (Sn_FSR1)
 * 0x0022   socket n tx read pointer (Sn_TX_RD0)
 * 0x0023   socket n tx read pointer (Sn_TX_RD1)
 * 0x0024   socket n tx write pointer (Sn_TX_WR0)
 * 0x0025   socket n tx write pointer (Sn_TX_WR1)
 * 0x0026   socket n rx received size (Sn_RX_RSR0)
 * 0x0027   socket n rx received size (Sn_RX_RSR1)
 * 0x0028   socket n rx read pointer (Sn_R_RD0)
 * 0x0029   socket n rx read pointer (Sn_R_RD1)
 * 0x002A   socket n rx write pointer (Sn_RX_WR0)
 * 0x002B   socket n rx write pointer (Sn_RX_WR1)
 * 0x002C   socket n interrupt mask (Sn_IMR)
 * 0x002D   socket n fragment offset in ip header (Sn_FRAG0)
 * 0x002E   socket n fragment offset in ip header (Sn_FRAG1)
 * 0x002F   keep alive timer (Sn_KPALVTR)
 * 0x0030 ... 0xFFFF reserved
 */
#include <stdint.h>
#include <stddef.h>

#include "socket.h"
#include "hal_spi.h"

struct skt_t {
    uint8_t         *dst_mac;
    uint8_t         *dst_ip;
    const uint16_t  base_addr;
    const uint16_t  tx_reg_addr;
    const uint16_t  rx_reg_addr;
    uint16_t        src_port;
    uint16_t        dst_port;
    const uint8_t   id;
    socket_sts      status;
    socket_protocol protocol;
    uint8_t         tx_memory_kb;
    uint8_t         rx_memory_kb;
    uint8_t         imr;
} skt_t;

#define SKT_SOCKET_BASE(s)  (((s * 4) + 1) << 3)
#define SKT_TX_BASE(s)      (((s * 4) + 2) << 3)
#define SKT_RX_BASE(s)      (((s * 4) + 3) << 3)

#define SKT_DEFAULT_INIT(_id) \
        .id             = _id,                  \
        .base_addr      = SKT_SOCKET_BASE(_id), \
        .tx_reg_addr    = SKT_TX_BASE(_id),     \
        .rx_reg_addr    = SKT_RX_BASE(_id),     \
        .src_port       = 0x0000,               \
        .status         = SOCKET_STS_CLOSED,    \
        .protocol       = 0,                    \
        .tx_memory_kb   = 2,                    \
        .rx_memory_kb   = 2,                    \

struct skt_t _skt[] = {
    {SKT_DEFAULT_INIT(0)},
    {SKT_DEFAULT_INIT(1)},
    {SKT_DEFAULT_INIT(2)},
    {SKT_DEFAULT_INIT(3)},
    {SKT_DEFAULT_INIT(4)},
    {SKT_DEFAULT_INIT(5)},
    {SKT_DEFAULT_INIT(6)},
    {SKT_DEFAULT_INIT(7)}
};

static void W5500_socket_write_reg(socket_id socket, socket_reg reg, uint8_t data)
{
    
}

static uint8_t W5500_socket_read_reg(socket_id socket, socket_reg reg)
{
    
}

static void W5500_socket_cmd(socket_id socket, socket_cmd cmd)
{
    W5500_spi_write_byte(ETH_SOCKET_BASE(socket) + BLOCK_SOCKET_0_REGISTER, SOCKET_REG_COMMAND, cmd);
}

#if 0
struct socket* W5500_socket(socket_id skt, socket_protocol mode)
{
    struct socket *skt_ptr = &this.socket[skt];
    
    W5500_spi_write_byte(ETH_SOCKET_BASE(skt) + BLOCK_SOCKET_0_REGISTER, SOCKET_REG_MODE, mode);
    skt_ptr->protocol = mode;
    
    if (SOCKET_MODE_TCP == mode) {
        skt_ptr->status = SOCKET_STS_INIT;
    } else if (SOCKET_MODE_UDP == mode) {
        skt_ptr->status = SOCKET_STS_UDP;
    } else { // SOCKET_MODE_MACRAW
        skt_ptr->status = SOCKET_STS_MACRAW;
    }
    
    return skt_ptr;
}
#endif

void W5500_socket_open(struct skt_t *skt, socket_id socket, socket_protocol mode)
{
    skt = &_skt[socket];
    skt->protocol = mode;
}

void W5500_socket_close(socket_id socket)
{
    W5500_socket_cmd(socket, SOCKET_CMD_CLOSE);
}

void W5500_socket_disconnect(socket_id socket)
{
    W5500_socket_cmd(socket, SOCKET_CMD_DISCONNECT);
}

void W5500_socket_listen(socket_id socket)
{
    W5500_socket_cmd(socket, SOCKET_CMD_LISTEN);
}

void W5500_socket_send(socket_id socket)
{
    W5500_socket_cmd(socket, SOCKET_CMD_SEND);
}

void W5500_socket_receive(socket_id socket, uint8_t *data, size_t data_size)
{

}

uint8_t W5500_socket_received_size(socket_id socket)
{
    return 0;
}

socket_sts W5500_socket_status(socket_id socket)
{
    
}


/* [] END OF FILE */
