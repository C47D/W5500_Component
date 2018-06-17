#include "udp.h"

uint8_t w5500_udp_open(uint16_t port)
{
#if 0
	uint8_t socket = 0;
    uint8_t status = ETH_SR_CLOSED;
    uint8_t tries = 0;
	
	do {
		socket = ETH_SocketOpen(port, ETH_PROTO_UDP);
	
		if (socket < ETH_MAX_SOCKETS) {
			ETH_Send(ETH_SREG_SR, ETH_SOCKET_BASE(socket), 0, &status, 1);
			if (status != ETH_SR_UDP) {
				ETH_SocketClose(socket,0);
				socket = 0xFF;
			}
		}
		++tries;
	} while ( (tries < 5) && ( status != ETH_SR_UDP ) );
	
	return socket;
#endif
}

uint16_t w5500_udp_send(socket_n socket, uint16_t ip, uint16_t port,
                        uint8_t *data, size_t data_size, uint8_t flags)
{
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

uint16_t w5500_udp_receive(socket_n socket, uint16_t ip, uint16_t port,
                           uint8_t *data, size_t data_size, uint8_t flags)
{
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

/**
 * \todo Open Multi-cast socket
 */

/**
 * \todo Send Multi-cast data
 */

/* [] END OF FILE */
