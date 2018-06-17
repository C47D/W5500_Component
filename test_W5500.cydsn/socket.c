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

static void W5500_socket_write_reg(socket_n socket, socket_reg reg, uint8_t data)
{
    
}

static uint8_t W5500_socket_read_reg(socket_n socket, socket_reg reg)
{
    
}

static void W5500_socket_cmd(socket_n socket, socket_cmd cmd)
{
    W5500_spi_write_byte(ETH_SOCKET_BASE(socket) + BLOCK_SOCKET_0_REGISTER, SOCKET_REG_COMMAND, cmd);
}

struct socket* W5500_socket(socket_n skt, socket_mode mode)
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

void W5500_socket_close(socket_n socket)
{
    W5500_socket_cmd(socket, SOCKET_CMD_CLOSE);
}

void W5500_socket_disconnect(socket_n socket)
{
    W5500_socket_cmd(socket, SOCKET_CMD_DISCONNECT);
}

void W5500_socket_listen(socket_n socket)
{
    W5500_socket_cmd(socket, SOCKET_CMD_LISTEN);
}

void W5500_socket_send(socket_n socket)
{
    W5500_socket_cmd(socket, SOCKET_CMD_SEND);
}

void W5500_socket_receive(socket_n socket, uint8_t *data, size_t data_size)
{

}

uint8_t W5500_socket_received_size(socket_n socket)
{
    return 0;
}

socket_sts W5500_socket_status(socket_n socket)
{
    
}


/* [] END OF FILE */
