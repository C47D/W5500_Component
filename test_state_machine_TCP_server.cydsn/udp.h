#if !defined(UDP_H_)
#define UDP_H_

#include "common.h"

uint8_t w5500_udp_open(uint16_t port);
uint16_t w5500_udp_send(socket_n socket, uint16_t ip, uint16_t port,
                        uint8_t *data, size_t data_size, uint8_t flags);
uint16_t w5500_udp_receive(socket_n socket, uint16_t ip, uint16_t port,
                           uint8_t *data, size_t data_size, uint8_t flags);

#endif /* udp_h_ */

/* [] END OF FILE */
