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

/* To be investigated:
- All the flags on the mode register

- Registers:

- Maximum segment size
  of TCP, and the register displays MSS set by the other party when TCP
  is activated in passive mode

- IP TOS
  Type of Service in IP header. It is set before OPEN Command

- IP TTL
  Time To Live field in IP header. It is set before OPEN Command

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
#ifndef `$INSTANCE_NAME`_SOCKET_H_
#define `$INSTANCE_NAME`_SOCKET_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
    
#include <CyLib.h> // CyDelay

#include "`$INSTANCE_NAME`_DEFS.h"
#include "`$INSTANCE_NAME`_UTILS.h"

struct skt_t;

// BSD Sockets
// int socket(int domain, int type, int protocol)
// domain = protocol family, PF_INET for UDP, TCP and other internet protocols
// type = SOCK_STREAM for TCP, SOCK_DGRAM for UDP
// protocol = almost always 0
//
// Client functions
// The client knows which server it is about to call, it's IP address, and it
// knows the port the server resides at.
// connect(int s, const struct sockaddr *name, socklen_t namelen)
//
// Server functions
// The typical server does not initiate the connection. Instead, it waits for
// a client to call it and request services. It does not know when the client
// will call, nor how many clients will call. It may be just sitting there,
// waiting patiently, one moment, the next moment, it can find itself swamped
// with requests from a number of clients, all calling at the same time.
//
// bind(int s, const struct sockaddr *addr, socklen_t addrlen)
// listen(int s, int backlog)
// accept(int s, struct sockaddr *addr, socklent_t *addrlen)

//* high level API *//
struct skt_t *`$INSTANCE_NAME`_socket_init(socket_id socket, const uint8_t mode_flags,
                                const socket_mem tx_mem, const socket_mem rx_mem);
// Client functions
// maybe add a timeout? and async version?
void `$INSTANCE_NAME`_socket_connect(struct skt_t *socket, const struct skt_addr_t *addr);
void `$INSTANCE_NAME`_socket_set_remote_address(struct skt_t *socket, const struct skt_addr_t *addr);

// Server functions
void `$INSTANCE_NAME`_socket_set_port(struct skt_t *socket, const uint16_t port);

// Common functions
void `$INSTANCE_NAME`_socket_write(struct skt_t *socket, const uint8_t *data, const size_t data_size);
void `$INSTANCE_NAME`_socket_read(struct skt_t *socket, uint8_t *data, const size_t data_size);

//* lower level API *//
socket_sts `$INSTANCE_NAME`_socket_get_status(struct skt_t *socket);
void `$INSTANCE_NAME`_socket_cmd(struct skt_t *socket, const socket_cmd cmd);

/* Set remote information, replaced with socket_connect */
void `$INSTANCE_NAME`_socket_set_remote_ip(struct skt_t *socket, struct ip_addr *ip);
void `$INSTANCE_NAME`_socket_set_remote_port(struct skt_t *socket, uint16_t port);
/* May be usefull, may be not */
void `$INSTANCE_NAME`_socket_get_remote_mac(struct skt_t *socket, struct mac_addr *mac);
void `$INSTANCE_NAME`_socket_get_remote_ip(struct skt_t *socket, struct ip_addr *ip);
uint16_t `$INSTANCE_NAME`_socket_get_remote_port(struct skt_t *socket);
/* This will be exposed */
void `$INSTANCE_NAME`_socket_get_remote_info(struct skt_t *socket, struct mac_addr *mac,
    struct ip_addr *ip, uint16_t *port);

/* Socket Interrupt Mask */
void `$INSTANCE_NAME`_socket_set_imr(struct skt_t *socket, uint8_t flags);
uint8_t `$INSTANCE_NAME`_socket_get_imr(struct skt_t *socket);

/* Socket Interrupt */
void `$INSTANCE_NAME`_socket_clear_interrupt_flag(struct skt_t *socket, uint8_t flags);
uint8_t `$INSTANCE_NAME`_socket_get_interrupt_flag(struct skt_t *socket);

/* Socket Rx and Tx buffer API */

// Sn_TX_FSR (Socket n TX Free Size Regisiter) (Read only)
uint16_t `$INSTANCE_NAME`_socket_get_tx_free_size(struct skt_t *socket);
uint16_t `$INSTANCE_NAME`_socket_get_received_data_size(struct skt_t *socket);

#endif /* socket_h_ */

/* [] END OF FILE */
