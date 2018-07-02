/* Un socket puede ser representado por:
 *
 *                                      Cambian sin             Cambian con
 *                  Constante   intervencion del usuario    intervencion del usuario
 * - ID (0 ... 7)       *
 * - Base address       *
 * - Status                                 *
 * - Memoria RX                                                     *
 * - Memoria TX                                                     *
 * - Protocolo                                                      *
 * - Source port                                                    *
 * - Destination port                       *
 * - Destination MAC                        *
 * - Destination IP                         *
 * - Interrupt Mask                                                 *
 * - Bytes recibidos                        *
 * - Direccion a leer                       *
 * - Espacio libre para xfer                *
 * - How much have we advanced rx_rd        *
 * - Interrupt Mask Register                                        *
 * - Interrupt flags                        *
 *
 * Por lo tanto un socket pudiera representarse por el struct:
 * struct socket {
 *      uint16_t    skt_base_address;
 *      uint16_t    skt_src_port;
 *      uint16_t    skt_dst_port;
 *      uint8_t     skt_id;
 *      uint8_t     skt_status;
 *      uint8_t     skt_protocol;
 *      uint8_t     skt_tx_memory_kb;
 *      uint8_t     skt_rx_memory_kb;
 *      uint8_t     skt_imr;
 * }
 */
#if !defined(SOCKET_H_)
#define SOCKET_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "common.h"

struct skt_t;

struct skt_t *W5500_socket_init(socket_id socket, const uint8_t mode_flags,
                                const socket_mem tx_mem, const socket_mem rx_mem);

void W5500_socket_set_port(struct skt_t *socket, const uint16_t port);
void W5500_socket_cmd(struct skt_t *socket, const socket_cmd cmd);

/* Socket Status Register */
socket_sts W5500_socket_get_status(struct skt_t *socket);

/*  TODO: Make this function static */
void W5500_socket_receive(struct skt_t *socket, uint8_t *data, size_t data_size);
uint16_t W5500_socket_get_received_data_size(struct skt_t *socket);
void W5500_socket_transmit(struct skt_t *socket, uint8_t *data, size_t data_size);
uint16_t W5500_socket_get_transmit_free(struct skt_t *socket);

uint16_t W5500_socket_tx_free_size(struct skt_t *socket);
uint16_t W5500_socket_tx_read_ptr(struct skt_t *socket);
uint16_t W5500_socket_tx_write_ptr(struct skt_t *socket);
void W5500_socket_tx_update_write_ptr(struct skt_t *socket, uint16_t new_addr);

uint16_t W5500_socket_rx_rcv_size(struct skt_t *socket);
uint16_t W5500_socket_rx_read_ptr(struct skt_t *socket);
void W5500_socket_rx_update_read_ptr(struct skt_t *socket, uint16_t new_addr);
uint16_t W5500_socket_rx_write_ptr(struct skt_t *socket);

/* Only expose this functions */
void W5500_socket_write(struct skt_t *socket, uint16_t addr, const uint8_t *data, const size_t data_size);
void W5500_socket_read(struct skt_t *socket, uint16_t addr, uint8_t *data, const size_t data_size);

/* Get information from the remote device, this functions will become static */
void W5500_socket_get_remote_mac(struct skt_t *socket, uint8_t *mac, size_t mac_size);
void W5500_socket_get_remote_ip(struct skt_t *socket, uint8_t *ip, size_t ip_size);
uint16_t W5500_socket_get_remote_port(struct skt_t *socket);
/* This will be exposed */
void W5500_socket_get_remote_info(struct skt_t *socket, uint8_t *mac, uint8_t *ip, uint16_t *port);

/* Socket Interrupt Mask */
void W5500_socket_set_imr(struct skt_t *socket, uint8_t flags);
uint8_t W5500_socket_get_imr(struct skt_t *socket);

/* Socket Interrupt */
void W5500_socket_clear_interrupt(struct skt_t *socket, uint8_t flags);
/* Aqui debemos de tener en cuenta el IMR para solo devolver interrupciones que
estan habilitadas, tal vez en un enum? */
uint8_t W5500_socket_get_interrupt(struct skt_t *socket);

/* To understand
- All the flags on the mode register

- Registers:

- Maximum segment size
  of TCP, and the register displays MSS set by the other party when TCP
  is activated in passive mode

- IP TOS
  Type of Service in IP header. It is set before OPEN Command

- IP TTL
  Time To Live field in IP header. It is set before OPEN Command

- Receive buffer size
- Transmit buffer size
- TX Free size [2]
- TX Read Pointer [2]
- TX Write Pointer [2]
- RX Received size [2]
- RX Read Pointer [2]
- RX Write Pointer [2]
- Fragment Offset in IP header [2]
  Fragment Field in IP header

- KAT (Keep alive timer)
  configured the transmitting timer of KEEP ALIVE packet of socket n
  it is valid only in TCP mode, and ignored in other modes. The time unit
  is 5s.

  KA packet is transmittable after Sn_SR is changed to SOCK_ESTABLISHED and
  after the data is transmitted or received to/from a peer at least once.

  if KAT is 0, an KA packet can be transmitted by SEND_KEEP command by the
  host (Manual keep alive proccess)
*/

#endif /* socket_h_ */

/* [] END OF FILE */
