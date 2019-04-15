#include <stdint.h>
#include <stddef.h>

#include "`$INSTANCE_NAME`_SOCKET.h"
#include "`$INSTANCE_NAME`_HAL.h"
#include "`$INSTANCE_NAME`_UTILS.h"

/* Macro helpers to calculate the socket register addresses */
#define SKT_SOCKET_BASE(s)  ((s * 4) + 1)
#define SKT_TX_BASE(s)      ((s * 4) + 2)
#define SKT_RX_BASE(s)      ((s * 4) + 3)

struct skt_t {
    /* Generic information */
    socket_id       id;
    socket_sts      status;
    socket_protocol protocol;
    socket_mem      tx_memory_kb;
    socket_mem      rx_memory_kb;
    /* Registers addresses */
    uint8_t         reg_base_addr;
    uint8_t         reg_tx_addr;
    uint8_t         reg_rx_addr;
    /* Rx/Tx pointers */
    uint16_t        tx_write_ptr;
    uint16_t        rx_read_ptr;
    /* Interrupt Mask register */
    uint8_t         imr;
} skt_t;

#if 0 // Legacy, to be removed.
/* Set the _id socket structs to default values */
#define SKT_DEFAULT_INIT(_id) \
{   \
    .id             = _id,                      \
    .status         = SOCKET_STS_CLOSED,        \
    .protocol       = SOCKET_PROTOCOL_CLOSED,   \
    .tx_memory_kb   = SOCKET_MEM_2KB,           \
    .rx_memory_kb   = SOCKET_MEM_2KB,           \
    .reg_base_addr  = SKT_SOCKET_BASE(_id),     \
    .reg_tx_addr    = SKT_TX_BASE(_id),         \
    .reg_rx_addr    = SKT_RX_BASE(_id),         \
    .imr            = 0,                        \
}\

struct skt_t _skt[] = {
    SKT_DEFAULT_INIT(0),
    SKT_DEFAULT_INIT(1),
    SKT_DEFAULT_INIT(2),
    SKT_DEFAULT_INIT(3),
    SKT_DEFAULT_INIT(4),
    SKT_DEFAULT_INIT(5),
    SKT_DEFAULT_INIT(6),
    SKT_DEFAULT_INIT(7)
};
#endif

static inline uint16_t `$INSTANCE_NAME`_socket_rx_rcv_size(struct skt_t *socket);
static inline uint16_t `$INSTANCE_NAME`_socket_rx_read_ptr(struct skt_t *socket);
static inline uint16_t `$INSTANCE_NAME`_socket_rx_write_ptr(struct skt_t *socket);
static inline void `$INSTANCE_NAME`_socket_rx_update_read_ptr(struct skt_t *socket, uint16_t new_addr);
static inline uint16_t `$INSTANCE_NAME`_socket_tx_free_size(struct skt_t *socket);
static inline uint16_t `$INSTANCE_NAME`_socket_tx_read_ptr(struct skt_t *socket);
static inline uint16_t `$INSTANCE_NAME`_socket_tx_write_ptr(struct skt_t *socket);
static inline void `$INSTANCE_NAME`_socket_tx_update_write_ptr(struct skt_t *socket, uint16_t new_addr);
static inline void `$INSTANCE_NAME`_socket_cmd_wait_for_completion(struct skt_t *socket);

struct skt_t *`$INSTANCE_NAME`_socket_init(socket_id socket, const uint8_t mode_flags,
                                const socket_mem tx_mem, const socket_mem rx_mem)
{
    struct skt_t *tmp = NULL;
    
    /* Check if the socket_id is a valid one */
    if (8 <= socket) {
        return NULL;
    }

    /* Calculate socket register addresses first */
    tmp->reg_base_addr  = SKT_SOCKET_BASE(socket);
    tmp->reg_tx_addr    = SKT_TX_BASE(socket);
    tmp->reg_rx_addr    = SKT_RX_BASE(socket);
    
    /* Updating fields based on W5500 registers contents */
    tmp->imr = `$INSTANCE_NAME`_spi_read_byte(tmp->reg_base_addr, SOCKET_REG_INTERRUPT_MASK);
    tmp->status = `$INSTANCE_NAME`_spi_read_byte(tmp->reg_base_addr, SOCKET_REG_STATUS);
    
    /* the socket shouldn't been used */
    if (SOCKET_STS_CLOSED != tmp->status) {
        return NULL;
    }
    
    /* Updating fields that can be changed only by the user and not the chip itself. */
    tmp->id             = socket;
    tmp->protocol       = mode_flags & 0x0F;
    tmp->tx_memory_kb   = tx_mem;
    tmp->rx_memory_kb   = rx_mem;
    
    /* only socket 0 can be used in MACRAW mode */
    if ((0 != socket) && (SOCKET_PROTOCOL_MACRAW & mode_flags)) {
        return NULL;
    }
    
    /* Update the registers according to the configuration */
    `$INSTANCE_NAME`_spi_write_byte(tmp->reg_base_addr, SOCKET_REG_MODE, mode_flags);
    `$INSTANCE_NAME`_spi_write_byte(tmp->reg_base_addr, SOCKET_REG_TX_BUFFER_SIZE, tmp->tx_memory_kb);
    `$INSTANCE_NAME`_spi_write_byte(tmp->reg_base_addr, SOCKET_REG_RX_BUFFER_SIZE, tmp->rx_memory_kb);
    
    return tmp;
}

void `$INSTANCE_NAME`_socket_set_remote_address(struct skt_t *socket, const struct skt_addr_t *addr)
{
    uint8_t _port[] = {((addr->port & 0xFF00) >> 8), (addr->port & 0x00FF)};

    `$INSTANCE_NAME`_spi_write(socket->reg_base_addr, SOCKET_REG_DESTINATION_PORT0,
        _port, sizeof(_port));
#if 1
    `$INSTANCE_NAME`_spi_write(socket->reg_base_addr, SOCKET_REG_DESTINATION_IP_ADDR0,
        addr->addr.octect, sizeof(addr->addr.octect));
#else
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_DESTINATION_IP_ADDR0, addr->addr.octect[0]);
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_DESTINATION_IP_ADDR1, addr->addr.octect[1]);
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_DESTINATION_IP_ADDR2, addr->addr.octect[2]);
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_DESTINATION_IP_ADDR3, addr->addr.octect[3]);
#endif
}

void `$INSTANCE_NAME`_socket_set_port(struct skt_t *socket, const uint16_t port)
{
    // uint8_t _port[] = {((port & 0xFF00) >> 8), (port & 0x00FF)};
    uint8_t _port[] = {port >> 8, port & 0xFF};
    
    `$INSTANCE_NAME`_spi_write(socket->reg_base_addr, SOCKET_REG_SOURCE_PORT0, _port, sizeof(_port));
}

void `$INSTANCE_NAME`_socket_cmd(struct skt_t *socket, const socket_cmd cmd)
{
    /* send the command */
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_COMMAND, cmd);
    /* wait for the command register to become 0, so then we know the command has been executed */
    `$INSTANCE_NAME`_socket_cmd_wait_for_completion(socket);
}

socket_sts `$INSTANCE_NAME`_socket_get_status(struct skt_t *socket)
{
    socket->status = (socket_sts) `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_STATUS);
    return socket->status;
}

uint16_t `$INSTANCE_NAME`_socket_get_received_data_size(struct skt_t *socket)
{
    uint8_t rcv_data_size[2] = {0};
    
    `$INSTANCE_NAME`_spi_read(socket->reg_base_addr, SOCKET_REG_RX_RECEIVED_SIZE0, rcv_data_size, 2);
    
    // TODO: Update the rx_received pointer?
    
    return (rcv_data_size[0] << 8) | rcv_data_size[1];
}

void `$INSTANCE_NAME`_socket_write(struct skt_t *socket, const uint8_t *data, const size_t data_size)
{
    uint16_t tx_ptr = 0;
    
    // 1. Get current tx_write_ptr position
    tx_ptr = `$INSTANCE_NAME`_socket_tx_write_ptr(socket);
    // 2. write the data into the tx reg
    `$INSTANCE_NAME`_spi_write(socket->reg_tx_addr, tx_ptr, data, data_size);
    // 3. update the tx_write_ptr position
    tx_ptr += data_size;
    `$INSTANCE_NAME`_socket_tx_update_write_ptr(socket, tx_ptr);
    // 4. send the SEND command
    `$INSTANCE_NAME`_socket_cmd(socket, SOCKET_CMD_SEND);
#if 0
uint16_t ETH_TcpSend( uint8_t socket, uint8_t* buffer, uint16_t len, uint8_t flags)
{
	uint16_t tx_length = 0;
    uint16_t max_packet = 0;
    uint16_t ptr = 0;
	uint8_t buf_size = 0;
    uint8_t result = 0;
	
	if ( ETH_SOCKET_BAD( socket ) ) {
        return 0;
    }
	
	tx_length = ETH_TxBufferFree( socket );
    
	if ( (tx_length < len ) && ((flags&ETH_TXRX_FLG_WAIT) != 0) ) {
        
		// there is not enough room in the buffer, but the caller requested
		// this to block until there was free space. So, check the memory
		// size to determine if the tx buffer is big enough to handle the
		// data block without fragmentation.
		ETH_Send(ETH_SREG_TXBUF_SIZE, ETH_SOCKET_BASE(socket),0,&buf_size,1);
		max_packet = (buf_size == 0)? 0 : (0x400 << (buf_size-1));
        
		// now that we know the max buffer size, if it is smaller than the
		// requested transmit lenght, we have an error, so return 0
		if (max_packet < len ) return 0;
		// otherwise, we will wait for the room in the buffer
		do {
			tx_length = ETH_TxBufferFree( socket );
		} while ( tx_length < len );
	} else {
		tx_length = len;
	}
    
	// The length of the Tx data block has now been determined, and can be
	// copied in to the W5500 buffer memory. First read the pointer, then
	// write data from the pointer forward, lastly update the pointer and issue
	// the SEND command.
	ETH_Send( ETH_SREG_TX_WR, ETH_SOCKET_BASE(socket),0,(uint8*)&ptr,2);
	ptr = CYSWAP_ENDIAN16( ptr );
	ETH_Send( ptr, ETH_TX_BASE(socket),1,buffer,tx_length);
	ptr += tx_length;
	ptr = CYSWAP_ENDIAN16( ptr );
	ETH_Send(ETH_SREG_TX_WR, ETH_SOCKET_BASE(socket),1,(uint8*)&ptr,2);
	
	ETH_ExecuteSocketCommand( socket, ETH_CR_SEND );
	
	if ( 0 != ( flags & ETH_TXRX_FLG_WAIT ) ) {
		// block until send is complete
		do {
			CyDelay(1);
			result = ETH_SocketSendComplete( socket );
		} while ( ( CYRET_FINISHED != result ) && ( CYRET_CANCELED != result ) );
	}
	
	return tx_length;
}
#endif
}

/* Read received Data from Sn_RX_BUF */
//void W5500_socket_read(struct skt_t *socket, uint16_t addr, uint8_t *data, const size_t data_size)
void `$INSTANCE_NAME`_socket_read(struct skt_t *socket, uint8_t *data, const size_t data_size)
{
    uint16_t rcv_size = 0;
    uint16_t rcv_ptr = 0;
    
    // 1. Get received data size
    rcv_size = `$INSTANCE_NAME`_socket_rx_rcv_size(socket);
    // 2. read rx reg
    `$INSTANCE_NAME`_spi_read(socket->reg_rx_addr, rcv_ptr, data, data_size);
    // 3. update received pointer
    rcv_ptr += rcv_size;
    `$INSTANCE_NAME`_socket_rx_update_read_ptr(socket, rcv_ptr);
    // 4. send RECV command
    `$INSTANCE_NAME`_socket_cmd(socket, SOCKET_CMD_RECV);
#if 0
uint16_t ETH_TcpReceive(uint8_t socket, uint8_t* buffer, uint16_t len, uint8_t flags)
{
	uint16_t rx_size = 0;
    uint16_t ptr = 0;
    uint16_t bytes = 0;
	
	// when there is a bad socket, just return 0 bys no matter what.
	if ( ETH_SOCKET_BAD( socket ) ) {
        return 0;
    }

	// Otherwise, read the number of bytes waiting to be read.  When the byte
	// count is less than the requested bytes, wait for them to be available
	// when the wait flag is set, otherwise, just read the waiting data once.
	do {
		rx_size = ETH_RxDataReady( socket );
	} while ( ( rx_size < len ) && ( flags & ETH_TXRX_FLG_WAIT ) );
	
	// When data is available, begin processing the data
	if ( 0 < rx_size ) { 
		// calculate the number of bytes to receive using the available data
		// and the requested length of data.
		bytes = ( rx_size > len ) ? len : rx_size;
		// Read the starting memory pointer address, and endian correct
		ETH_Send( ETH_SREG_RX_RD, ETH_SOCKET_BASE(socket),0,(uint8*)&ptr,2);
		ptr = CYSWAP_ENDIAN16( ptr );
		// Retrieve the data bytes from the W5500 buffer
		ETH_Send( ptr, ETH_RX_BASE(socket),0,buffer,bytes);
 
		// Calculate the new buffer pointer location, endian correct, and
		// update the pointer register within the W5500 socket registers
		ptr += bytes;
		ptr = CYSWAP_ENDIAN16( ptr );
		ETH_Send(ETH_SREG_RX_RD, ETH_SOCKET_BASE(socket),1,(uint8*)&ptr,2);
		// when all of the available data was read from the message, execute the receive command
		ETH_ExecuteSocketCommand( socket, ETH_CR_RECV );
	}
    
	return bytes;
}

/**
 * @brief TCP Get char.
 *
 */
char ETH_TcpGetChar( uint8_t socket )
{
	char ch = 0;
	uint16_t len = 0;
	
	do {
		len = ETH_TcpReceive(socket, (uint8*)&ch, 1, 0);
	} while ( len < 1 );
    
	return ch;
}

/**
 * @brief TCP Get line.
 *
 */
int ETH_TcpGetLine( uint8_t socket, char *buffer )
{
	char ch = 0;
	int idx = 0;
	
	do {
		ch = ETH_TcpGetChar( socket );
		if ( (ch != '\r') && (ch!='\n') ) {
			if ( (ch == '\b')||(ch==127) ) {
				buffer[idx] = 0;
				idx = (idx == 0)?0:idx-1;
			} else {
				buffer[idx++] = ch;
				buffer[idx] = 0;
			}
		}
	} while ( (ch!='\r') && (ch!='\n'));
    
	buffer[idx] = 0;
	
	return idx;
}
    
    /======
    
    uint16_t new_reg_rx_addr = 0;
    uint16_t rcv_size = 0;
    
    uint8_t data_ptr[2] = {0};
    uint16_t data_addr = 0;
        
    rcv_size = `$INSTANCE_NAME`_socket_get_received_data_size(tcp_server);
    
    if (0 < rcv_size) {
        
        /* Read Sn_RX_RD (Socket n RX Read Pointer) to get RX Data Start Address */
        // TODO: Add in the socket functions
        data_ptr[0] = `$INSTANCE_NAME`_spi_read_byte(1, SOCKET_REG_RX_READ_POINTER0);
        data_ptr[1] = `$INSTANCE_NAME`_spi_read_byte(1, SOCKET_REG_RX_READ_POINTER1);
        data_addr = (data_ptr[0] << 8 | data_ptr[1]);

        /* Read received Data from Sn_RX_BUF */
        // TODO: Add in the socket functions
        `$INSTANCE_NAME`_spi_read(0x00003, data_addr, data, rcv_size);
            
        /* Despues de leer los datos tenemos que actualizar Sn_RX_RD con el
        * tamaño de los datos que leimos. Mandar el comando RECV para notificar
        * al W5500. */
        // TODO: Add in the socket functions
        new_reg_rx_addr = data_addr + rcv_size;
        data_ptr[0] = (new_reg_rx_addr & 0xFF00) >> 8;
        data_ptr[1] = new_reg_rx_addr & 0xFF;
        
        /* update receive address */
        `$INSTANCE_NAME`_spi_write_byte(1, SOCKET_REG_RX_READ_POINTER0, data_ptr[0]);
        `$INSTANCE_NAME`_spi_write_byte(1, SOCKET_REG_RX_READ_POINTER1, data_ptr[1]);
        
        `$INSTANCE_NAME`_socket_cmd_recv(tcp_server);
        
        data[rcv_size] = '\0';
        snprintf(data_str, sizeof(data_str), "%s", data);
    }
#endif
}

/* maybe remove remote ip and mac from the struct, or just update them
 when detecting a remote connection? */
void `$INSTANCE_NAME`_socket_get_remote_mac(struct skt_t *socket, struct mac_addr *mac)
{
    `$INSTANCE_NAME`_spi_read(socket->reg_base_addr, SOCKET_REG_DESTINATION_HW_ADDR0,
        mac->octect, sizeof(mac->octect));
}

void `$INSTANCE_NAME`_socket_get_remote_ip(struct skt_t *socket, struct ip_addr *ip)
{
    `$INSTANCE_NAME`_spi_read(socket->reg_base_addr, SOCKET_REG_DESTINATION_IP_ADDR0,
        ip->octect, sizeof(ip->octect));
}

uint16_t `$INSTANCE_NAME`_socket_get_remote_port(struct skt_t *socket)
{
    uint8_t port[2] = {0};
    
    `$INSTANCE_NAME`_spi_read(socket->reg_base_addr, SOCKET_REG_DESTINATION_PORT0,
        port, sizeof(port));
    
    return (port[0] << 8) | (port[1]);
}

void `$INSTANCE_NAME`_socket_set_remote_ip(struct skt_t *socket, struct ip_addr *ip)
{
#if 0
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_DESTINATION_IP_ADDR0, ip->octect[0]);
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_DESTINATION_IP_ADDR1, ip->octect[1]);
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_DESTINATION_IP_ADDR2, ip->octect[2]);
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_DESTINATION_IP_ADDR3, ip->octect[3]);
#else
    `$INSTANCE_NAME`_spi_write(socket->reg_base_addr, SOCKET_REG_DESTINATION_IP_ADDR0,
        ip->octect, sizeof(ip->octect));
#endif
}

void `$INSTANCE_NAME`_socket_set_remote_port(struct skt_t *socket, uint16_t port)
{
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_DESTINATION_PORT0, port >> 8);
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_DESTINATION_PORT1, port & 0xFF);
}

void `$INSTANCE_NAME`_socket_set_imr(struct skt_t *socket, uint8_t flags)
{
    socket->imr = flags;
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_INTERRUPT_MASK, flags);
}

uint8_t `$INSTANCE_NAME`_socket_get_imr(struct skt_t *socket)
{
    return socket->imr;
}

// clear one or more interrupt flags at once?
void `$INSTANCE_NAME`_socket_clear_interrupt_flag(struct skt_t *socket, uint8_t flags)
{
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_INTERRUPT, flags);
}

/* imr & interrupt_status_register to get only the enabled interrupts */
uint8_t `$INSTANCE_NAME`_socket_get_interrupt_flag(struct skt_t *socket)
{
    return `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_INTERRUPT);
}

/* static functions */
static inline uint16_t `$INSTANCE_NAME`_socket_tx_free_size(struct skt_t *socket)
{
    uint16_t result = 0;
    uint8_t free_0 = 0;
    uint8_t free_1 = 0;
    
    free_0 = `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_TX_FREE_SIZE0);
    free_1 = `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_TX_FREE_SIZE1);
    
    result = (free_0 << 8) | free_1;
    
    return result;
}

static inline uint16_t `$INSTANCE_NAME`_socket_tx_read_ptr(struct skt_t *socket)
{
    uint8_t result[2] = {0};
#if 0
    uint8_t read_0 = 0;
    uint8_t read_1 = 0;
    
    read_0 = `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_TX_READ_POINTER0);
    read_1 = `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_TX_READ_POINTER1);
    result = (read_0 << 8) | read_1;
#else 
    `$INSTANCE_NAME`_spi_read(socket->reg_base_addr, SOCKET_REG_TX_READ_POINTER0, result, sizeof(result));
#endif
    
    return (result[0] << 8) | result[1];
}

static inline uint16_t `$INSTANCE_NAME`_socket_tx_write_ptr(struct skt_t *socket)
{
    uint16_t result = 0;
    uint8_t read_0 = 0;
    uint8_t read_1 = 0;
    
    read_0 = `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_TX_WRITE_POINTER0);
    read_1 = `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_TX_WRITE_POINTER1);
    
    result = (read_0 << 8) | read_1;
    
    return result;
}

static inline void `$INSTANCE_NAME`_socket_tx_update_write_ptr(struct skt_t *socket, uint16_t new_addr)
{
#if 0
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_TX_WRITE_POINTER0, (new_addr & 0xFF00) >> 8);
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_TX_WRITE_POINTER1, new_addr & 0x00FF);
#else
    `$INSTANCE_NAME`_spi_write(socket->reg_base_addr, SOCKET_REG_TX_WRITE_POINTER1,
        (uint8_t []){(new_addr & 0xFF00) >> 8, new_addr & 0x00FF}, 2);
#endif
}

static inline uint16_t `$INSTANCE_NAME`_socket_rx_rcv_size(struct skt_t *socket)
{
    uint8_t skt_rcv_data_size[2] = {};
#if 1
    `$INSTANCE_NAME`_spi_read(socket->reg_base_addr, SOCKET_REG_RX_RECEIVED_SIZE0,
        skt_rcv_data_size, sizeof(skt_rcv_data_size));
#else
    skt_rcv_data_size[0] = `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_RX_RECEIVED_SIZE0);
    skt_rcv_data_size[1] = `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_RX_RECEIVED_SIZE1);
#endif
    return (skt_rcv_data_size[0] << 8) | skt_rcv_data_size[1];
}

/* Read Sn_RX_RD (Socket n RX Read Pointer) to get RX Data Start Address */
static inline uint16_t `$INSTANCE_NAME`_socket_rx_read_ptr(struct skt_t *socket)
{
    uint16_t result = 0;
    uint8_t read_0 = 0;
    uint8_t read_1 = 0;
    
    read_0 = `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_RX_READ_POINTER0);
    read_1 = `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_RX_READ_POINTER1);
    
    result = (read_0 << 8) | read_1;
    
    return result;
}

static inline void `$INSTANCE_NAME`_socket_rx_update_read_ptr(struct skt_t *socket, uint16_t new_addr)
{
#if 0
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_RX_READ_POINTER0, (new_addr & 0xFF00) >> 8);
    `$INSTANCE_NAME`_spi_write_byte(socket->reg_base_addr, SOCKET_REG_RX_READ_POINTER1, new_addr & 0xFF);
#else
    `$INSTANCE_NAME`_spi_write(socket->reg_base_addr, SOCKET_REG_RX_READ_POINTER1,
        (uint8_t []){new_addr >> 8, new_addr & 0xFF}, 2);
#endif
    `$INSTANCE_NAME`_socket_cmd(socket, SOCKET_CMD_RECV);
}

static inline uint16_t `$INSTANCE_NAME`_socket_rx_write_ptr(struct skt_t *socket)
{
    uint16_t result = 0;
    uint8_t read_0 = 0;
    uint8_t read_1 = 0;
    
    read_0 = `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_RX_WRITE_POINTER0);
    read_1 = `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_RX_WRITE_POINTER1);
    
    result = (read_0 << 8) | read_1;
    
    return result;
}

static inline void `$INSTANCE_NAME`_socket_cmd_wait_for_completion(struct skt_t *socket)
{
    while (0 != `$INSTANCE_NAME`_spi_read_byte(socket->reg_base_addr, SOCKET_REG_COMMAND)) {
        CyDelay(5);
    }
}

/* [] END OF FILE */
