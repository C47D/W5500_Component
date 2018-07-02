/*
 * From http://ietf.org/rfc/rfc768.txt
 *
 * A user interface should allow:
 * - The creation of new receive ports.
 * - Receive operations on the receive ports that return the data octets and an
 *   indication of source port and source address.
 * - An operation that allows a datagram to be sent, specifying the data, source
 *   and destination ports and addresses to be sent.
 *
 *   IP Interface
 * - The UDP module must be able to determine the source and destination internet
 *   addresses and the protocol field from the internet header.
 *
 * struct udp_t {
 *     struct skt_t     skt;            // for souce port
 *     struct IP_ADDR   remote_ip_addr;
 *     uint16_t         remote_port;
 *     uint16_t         length;         // data + header data size
 *     uint16_t         checksum;       // 16bit one's complement of the one's complement sum
 *                                      // of a pseudo header of information from the IP header,
 *                                      // the UDP header, and the data, padded with zro octects
 *                                      // end (if necessary) to make a multiple of two octets.
 * } udp_t;
 *
 *
 * BSD Sockets
 * UDP is also allowed to establish a connection, though it is not required.
 * http://wiki.treck.com/Introduction_to_BSD_Sockets
 */

#if !defined(UDP_H_)
#define UDP_H_

#include "common.h"

struct udp_t;

struct udp_t *W5500_udp_open(socket_id socket, uint16_t port);
void W5500_udp_send(struct udp_t *udp, uint32_t ip, uint16_t port, uint8_t *buffer, uint16_t len);
void W5500_udp_rcv(struct udp_t *udp, uint32_t *header, uint8_t *buffer, uint16_t len);

#endif /* udp_h_ */

/* [] END OF FILE */
