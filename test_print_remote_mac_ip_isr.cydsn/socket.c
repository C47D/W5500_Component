#include "project.h"

#include <stdint.h>
#include <stddef.h>

#include "socket.h"
#include "hal_spi.h"

#define SKT_SOCKET_BASE(s)  ((s * 4) + 1)
#define SKT_TX_BASE(s)      ((s * 4) + 2)
#define SKT_RX_BASE(s)      ((s * 4) + 3)

struct skt_t {
    socket_id       id;
    socket_sts      status;
    socket_protocol protocol;
    socket_mem      tx_memory_kb;
    socket_mem      rx_memory_kb;
    uint16_t        src_port;
    uint8_t         base_addr;
    uint8_t         tx_addr;
    uint8_t         rx_addr;
    uint8_t         imr;
} skt_t;

/* Set the _id socket structs to default values */
#define SKT_DEFAULT_INIT(_id) \
{   \
    .id             = _id,                      \
    .status         = SOCKET_STS_CLOSED,        \
    .protocol       = SOCKET_PROTOCOL_CLOSED,   \
    .tx_memory_kb   = SOCKET_MEM_2KB,           \
    .rx_memory_kb   = SOCKET_MEM_2KB,           \
    .src_port       = 0,                        \
    .base_addr      = SKT_SOCKET_BASE(_id),     \
    .tx_addr        = SKT_TX_BASE(_id),         \
    .rx_addr        = SKT_RX_BASE(_id),         \
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

static inline void W5500_socket_write_reg(const uint16_t base, const socket_reg reg, const uint8_t data)
{
    W5500_spi_write_byte(base, reg, data);
}

static inline void W5500_socket_write_reg_buff(uint16_t base, socket_reg reg, const uint8_t *data, const size_t reg_size)
{
    W5500_spi_write(base, reg, data, reg_size);
}

static inline uint8_t W5500_socket_read_reg(const uint16_t base, const socket_reg reg)
{
    return W5500_spi_read_byte(base, reg);
}

static inline void W5500_socket_read_reg_buff(const uint16_t base, const socket_reg reg, uint8_t *data, const size_t reg_size)
{
    W5500_spi_read(base, reg, data, reg_size);
}

static inline void W5500_socket_cmd_wait_for_completion(struct skt_t *socket)
{
    while (W5500_socket_read_reg(socket->base_addr, SOCKET_REG_COMMAND)) {
    }
}

struct skt_t *W5500_socket_init(socket_id socket, const uint8_t mode_flags,
                                const socket_mem tx_mem, const socket_mem rx_mem)
{
    struct skt_t *tmp = &_skt[socket];
    
    if ((SOCKET_STS_CLOSED != tmp->status) || (8 <= socket)) {
        return NULL;
    }
    
    // Settings...
    //tmp->base_addr      = SKT_SOCKET_BASE(socket); // <- may be not necessary, it's already initialized
    //tmp->tx_addr        = SKT_TX_BASE(socket); // <- may be not necessary
    //tmp->rx_addr        = SKT_RX_BASE(socket); // <- may be not necessary
    //tmp->status         = SOCKET_STS_CLOSED; // <- may be not necessary
    tmp->protocol       = mode_flags & 0x0F;
    tmp->tx_memory_kb   = tx_mem;
    tmp->rx_memory_kb   = rx_mem;
    
    // both can be uint16_t and should be swapped on every operation
    // get current tx_write_ptr, it should be 0
    // get current rx_write_ptr, it should be 0
    
    W5500_socket_write_reg(tmp->base_addr, SOCKET_REG_MODE, mode_flags);
    W5500_socket_write_reg(tmp->base_addr, SOCKET_REG_TX_BUFFER_SIZE, tmp->tx_memory_kb);
    W5500_socket_write_reg(tmp->base_addr, SOCKET_REG_RX_BUFFER_SIZE, tmp->rx_memory_kb);
    
    return tmp;
}

void W5500_socket_set_port(struct skt_t *socket, const uint16_t port)
{
    socket->src_port = port;
    uint8_t _port[] = {((port & 0xFF00) >> 8), (port & 0x00FF)};
    
    W5500_socket_write_reg_buff(socket->base_addr, SOCKET_REG_SOURCE_PORT0, _port, sizeof(_port));
}

void W5500_socket_cmd(struct skt_t *socket, const socket_cmd cmd)
{
    W5500_socket_write_reg(socket->base_addr, SOCKET_REG_COMMAND, cmd);
    W5500_socket_cmd_wait_for_completion(socket);
}

socket_sts W5500_socket_get_status(struct skt_t *socket)
{
    socket->status = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_STATUS);
    return socket->status;
}

void W5500_socket_receive(struct skt_t *socket, uint8_t *data, size_t data_size)
{
    (void)socket;
    (void)data;
    (void)data_size;
}

uint16_t W5500_socket_get_received_data_size(struct skt_t *socket)
{
    uint8_t skt_rcv_data_size[2] = {0};
    
    skt_rcv_data_size[0] = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_RX_RECEIVED_SIZE0);
    skt_rcv_data_size[1] = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_RX_RECEIVED_SIZE1);
    return (skt_rcv_data_size[0] << 8) | skt_rcv_data_size[1];
}

void W5500_socket_transmit(struct skt_t *socket, uint8_t *data, size_t data_size)
{
    (void)socket;
    (void)data;
    (void)data_size;
}

uint16_t W5500_socket_get_transmit_free(struct skt_t *socket)
{
    (void)socket;
    
    return 0;
}

/* new api */
uint16_t W5500_socket_tx_free_size(struct skt_t *socket)
{
    uint16_t result = 0;
    uint8_t free_0 = 0;
    uint8_t free_1 = 0;
    
    free_0 = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_TX_FREE_SIZE0);
    free_1 = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_TX_FREE_SIZE1);
    
    result = (free_0 << 8) | free_1;
    
    return result;
}

uint16_t W5500_socket_tx_read_ptr(struct skt_t *socket)
{
    uint16_t result = 0;
    uint8_t read_0 = 0;
    uint8_t read_1 = 0;
    
    read_0 = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_TX_READ_POINTER0);
    read_1 = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_TX_READ_POINTER1);
    
    result = (read_0 << 8) | read_1;
    
    return result;
}

uint16_t W5500_socket_tx_write_ptr(struct skt_t *socket)
{
    uint16_t result = 0;
    uint8_t read_0 = 0;
    uint8_t read_1 = 0;
    
    read_0 = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_TX_WRITE_POINTER0);
    read_1 = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_TX_WRITE_POINTER1);
    
    result = (read_0 << 8) | read_1;
    
    return result;
}

void W5500_socket_tx_update_write_ptr(struct skt_t *socket, uint16_t new_addr)
{
    W5500_spi_write_byte(socket->base_addr, SOCKET_REG_TX_WRITE_POINTER0, (new_addr & 0xFF00) >> 8);
    W5500_spi_write_byte(socket->base_addr, SOCKET_REG_TX_WRITE_POINTER1, new_addr & 0x00FF);
}

uint16_t W5500_socket_rx_rcv_size(struct skt_t *socket)
{
    uint8_t skt_rcv_data_size[2] = {0};
    
    skt_rcv_data_size[0] = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_RX_RECEIVED_SIZE0);
    skt_rcv_data_size[1] = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_RX_RECEIVED_SIZE1);
    return (skt_rcv_data_size[0] << 8) | skt_rcv_data_size[1];
}

/* Read Sn_RX_RD (Socket n RX Read Pointer) to get RX Data Start Address */
uint16_t W5500_socket_rx_read_ptr(struct skt_t *socket)
{
    uint16_t result = 0;
    uint8_t read_0 = 0;
    uint8_t read_1 = 0;
    
    read_0 = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_RX_READ_POINTER0);
    read_1 = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_RX_READ_POINTER1);
    
    result = (read_0 << 8) | read_1;
    
    return result;
}

void W5500_socket_rx_update_read_ptr(struct skt_t *socket, uint16_t new_addr)
{
    W5500_spi_write_byte(socket->base_addr, SOCKET_REG_RX_READ_POINTER0, (new_addr & 0xFF00) >> 8);
    W5500_spi_write_byte(socket->base_addr, SOCKET_REG_RX_READ_POINTER1, new_addr & 0xFF);
    W5500_socket_cmd(socket, SOCKET_CMD_RECV);
}

uint16_t W5500_socket_rx_write_ptr(struct skt_t *socket)
{
    uint16_t result = 0;
    uint8_t read_0 = 0;
    uint8_t read_1 = 0;
    
    read_0 = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_RX_WRITE_POINTER0);
    read_1 = W5500_spi_read_byte(socket->base_addr, SOCKET_REG_RX_WRITE_POINTER1);
    
    result = (read_0 << 8) | read_1;
    
    return result;
}

void W5500_socket_write(struct skt_t *socket, uint16_t addr, const uint8_t *data, const size_t data_size)
{
    W5500_spi_write(socket->tx_addr, addr, data, data_size);
    
#if 0
    
    So we can have a function (socket_send?) that does the following:
    1. Get current the tx_write_ptr position
    2. write the data into the tx reg
    3. update the tx_write_ptr position
    4. send the SEND command

    tx_ptr = W5500_socket_tx_write_ptr(tcp_server);
    W5500_socket_write(tcp_server, tx_ptr, (uint8_t *) status_code, strlen(status_code));
    tx_ptr += strlen(status_code);
    W5500_socket_tx_update_write_ptr(tcp_server, tx_ptr);
    W5500_socket_cmd_send(tcp_server);
    
/**
 * \brief Generic transmission of a data block using TCP
 * \param socket the socket that will be used to send the data
 * \param *buffer the array of data to be sent using TCP
 * \param len the length of data to send from the buffer
 * \param flags control flags for controlling options for transmission
 *
 * ETH_TcpSend transmits a block of generic data using TCP through a socket.
 * the connection must have been previously established in order for the
 * the function to operate properly, otherwise, no data will be transmitted.
 */
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

/* Keep addr as a member of the skt_t struct */
/* Read received Data from Sn_RX_BUF */
void W5500_socket_read(struct skt_t *socket, uint16_t addr, uint8_t *data, const size_t data_size)
{
    W5500_spi_read(socket->rx_addr, addr, data, data_size);
    
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
    
    uint16_t new_rx_addr = 0;
    uint16_t rcv_size = 0;
    
    uint8_t data_ptr[2] = {0};
    uint16_t data_addr = 0;
    
    LED_Write(~LED_Read());
        
    rcv_size = W5500_socket_get_received_data_size(tcp_server);
    
    if (0 < rcv_size) {
        
        /* Read Sn_RX_RD (Socket n RX Read Pointer) to get RX Data Start Address */
        // TODO: Add in the socket functions
        data_ptr[0] = W5500_spi_read_byte(1, SOCKET_REG_RX_READ_POINTER0);
        data_ptr[1] = W5500_spi_read_byte(1, SOCKET_REG_RX_READ_POINTER1);
        data_addr = (data_ptr[0] << 8 | data_ptr[1]);

        /* Read received Data from Sn_RX_BUF */
        // TODO: Add in the socket functions
        W5500_spi_read(0x00003, data_addr, data, rcv_size);
            
        /* Despues de leer los datos tenemos que actualizar Sn_RX_RD con el
        * tamaño de los datos que leimos. Mandar el comando RECV para notificar
        * al W5500. */
        // TODO: Add in the socket functions
        new_rx_addr = data_addr + rcv_size;
        data_ptr[0] = (new_rx_addr & 0xFF00) >> 8;
        data_ptr[1] = new_rx_addr & 0xFF;
        
        /* update receive address */
        W5500_spi_write_byte(1, SOCKET_REG_RX_READ_POINTER0, data_ptr[0]);
        W5500_spi_write_byte(1, SOCKET_REG_RX_READ_POINTER1, data_ptr[1]);
        
        W5500_socket_cmd_recv(tcp_server);
        
        data[rcv_size] = '\0';
        snprintf(data_str, sizeof(data_str), "%s", data);
    }
#endif
}

/* maybe remove remote ip and mac from the struct, also remote ip, or just update them
 when detecting a remote connection? */
void W5500_socket_get_remote_mac(struct skt_t *socket, uint8_t *mac, size_t mac_size)
{
    W5500_spi_read(socket->base_addr, SOCKET_REG_DESTINATION_HW_ADDR0, mac, mac_size);
}

void W5500_socket_get_remote_ip(struct skt_t *socket, uint8_t *ip, size_t ip_size)
{
    W5500_spi_read(socket->base_addr, SOCKET_REG_DESTINATION_IP_ADDR0, ip, ip_size);
}

uint16_t W5500_socket_get_remote_port(struct skt_t *socket)
{
    uint8_t port[2] = {0};
    
    W5500_spi_read(socket->base_addr, SOCKET_REG_DESTINATION_PORT0, port, sizeof(port));
    
    return (port[0] << 8) | (port[1]);
}

void W5500_socket_set_imr(struct skt_t *socket, uint8_t flags)
{
    socket->imr = flags;
    W5500_spi_write_byte(socket->base_addr, SOCKET_REG_INTERRUPT_MASK, flags);
}

uint8_t W5500_socket_get_imr(struct skt_t *socket)
{
    //return W5500_spi_read_byte(socket->base_addr, SOCKET_REG_INTERRUPT_MASK);
    return socket->imr;
}

void W5500_socket_clear_interrupt(struct skt_t *socket, uint8_t flags)
{
    W5500_spi_write_byte(socket->base_addr, SOCKET_REG_INTERRUPT, flags);
}

/* imr & interrupt_status_register to get only the enabled interrupts */
uint8_t W5500_socket_get_interrupt(struct skt_t *socket)
{
    return W5500_spi_read_byte(socket->base_addr, SOCKET_REG_INTERRUPT);
}

/* [] END OF FILE */
