#include "udp.h"
#include "socket.h"

// maybe we can setup another struct describing a udp
// socket, maybe it's just a wrapper around a socket +
// some interesting information

struct udp_t {
    struct skt_t    *socket;
};

extern struct skt_t _skt;

struct udp_t *W5500_udp_open(socket_id socket, uint16_t port)
{
    struct udp_t *udp = NULL;
    struct skt_t *skt = NULL;
    
    // socket "sanity" checks gets done in the socket funcs so i don't have to pollute
    // the "higher level" API
    skt = W5500_socket_init(socket, SOCKET_PROTOCOL_UDP, SOCKET_MEM_2KB, SOCKET_MEM_2KB);
    
    if (NULL != skt) {
        W5500_socket_set_port(skt, port);
        udp->socket = skt;
    }
    
    return udp;
}

void W5500_udp_send(struct udp_t *udp, uint32_t ip, uint16_t port, uint8_t *buffer, uint16_t len)
{
    (void)udp;
    (void)ip;
    (void)port;
    (void)buffer;
    (void)len;
    
#if 0
	uint16_t tx_length = ETH_GetTxLength(socket,len,flags);
	
	port = CYSWAP_ENDIAN16(port); // fix endian-ness
    
	// setup destination information
	ETH_Send(ETH_SREG_DIPR,ETH_SOCKET_BASE(socket),1,(uint8*)&ip,4);
	ETH_Send(ETH_SREG_DPORT, ETH_SOCKET_BASE(socket),1,(uint8*)&port,2);
	ETH_WriteTxData(socket, buffer, tx_length, flags);
	
	return tx_length;
#endif
}

void W5500_udp_rcv(struct udp_t *udp, uint32_t *header, uint8_t *buffer, uint16_t len)
{
    (void)udp;
    (void)header;
    (void)buffer;
    (void)len;
    
#if 0
	uint16_t rx_size = 0;
    uint16_t bytes = 0;
    uint16_t ptr = 0;
	
	// request the length of the data block available for reading, but, add
	// the header size (8 bytes) to the length of data requested to account
	// for the header sitting in the Rx Buffers.
	do {
		rx_size = ETH_RxDataReady( socket );
	} while ( ( 8 > rx_size ) && ( flags & ETH_TXRX_FLG_WAIT ) );
	
	// if there is data to read from the buffer...
	if ( 7 < rx_size ) {
		// calculate the number of bytes to receive using the available data
		// and the requested length of data.
		bytes = ( rx_size > len ) ? len : rx_size;
		// Read the starting memory pointer address, and endian correct
		ETH_Send( ETH_SREG_RX_RD, ETH_SOCKET_BASE(socket),0,(uint8*)&ptr,2);
		ptr = CYSWAP_ENDIAN16( ptr );
		// Read the UDP header block from the memory
		ETH_Send( ptr,ETH_RX_BASE(socket),0,header,8);
		ptr += 8;
		// read the number of bytes to read from the UDP header
		bytes = header[6];
		bytes = ( bytes << 8 ) + header[7];
		
		// Retrieve the length of data from the received UDP packet, starting
		// right after the end of the packet header.
		ETH_Send( ptr, ETH_RX_BASE(socket),0,buffer,bytes);
		// Calculate the new buffer pointer location, endian correct, and
		// update the pointer register within the W5500 socket registers
		ptr += bytes;
		ptr = CYSWAP_ENDIAN16( ptr );
		ETH_Send(ETH_SREG_RX_RD, ETH_SOCKET_BASE(socket),1,(uint8*)&ptr,2);
		// when all of the available data was read from the message, execute
		// the receive command
		ETH_ExecuteSocketCommand( socket, ETH_CR_RECV );
		
	}
	
	return bytes;
#endif
}

#if 0
/**
 * \todo Open Multi-cast socket
 */

/* Start EthernetUDP socket, listening at local port PORT */
uint8_t EthernetUDP::beginMulticast(IPAddress ip, uint16_t port)
{
	if (sockindex < MAX_SOCK_NUM) Ethernet.socketClose(sockindex);
	sockindex = Ethernet.socketBeginMulticast(SnMR::UDP | SnMR::MULTI, ip, port);
	if (sockindex >= MAX_SOCK_NUM) return 0;
	_port = port;
	_remaining = 0;
	return 1;
}

/**
 * \todo Send Multi-cast data
 */
#endif

/* [] END OF FILE */
