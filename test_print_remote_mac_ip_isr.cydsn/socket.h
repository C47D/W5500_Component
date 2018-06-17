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
 *
 * Por lo tanto un socket pudiera representarse por el struct:
 * struct socket {
 *      uint8_t         *skt_dst_mac;
 *      uint8_t         *skt_dst_ip;
 *      const uint16_t  skt_base_address;
 *      uint16_t        skt_src_port;
 *      uint16_t        skt_dst_port;
 *      const uint8_t   skt_id;
 *      uint8_t         skt_status;
 *      uint8_t         skt_protocol;
 *      uint8_t         skt_tx_memory_kb;
 *      uint8_t         skt_rx_memory_kb;
 *      uint8_t         skt_imr;
 * }
 */
#if !defined(SOCKET_H_)
#define SOCKET_H_

#include <stdint.h>
#include <stddef.h>

#include "common.h"

/* This array represents the 8 W5500 sockets */
extern struct W5500 this;

// struct socket* W5500_socket(socket_id skt, socket_protocol mode);
struct skt_t;

void W5500_socket_open(struct skt_t *skt, socket_id socket, socket_protocol mode);
void W5500_socket_close(socket_id socket);
void W5500_socket_disconnect(socket_id socket);
void W5500_socket_listen(socket_id socket);
void W5500_socket_send(socket_id socket);
void W5500_socket_receive(socket_id socket, uint8_t *data, size_t data_size);
uint8_t W5500_socket_received_size(socket_id socket);
socket_sts W5500_socket_status(socket_id socket);

#endif /* socket_h_ */

/* [] END OF FILE */
