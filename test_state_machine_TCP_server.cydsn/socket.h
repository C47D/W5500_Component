#if !defined(SOCKET_H_)
#define SOCKET_H_

#include <stdint.h>
#include <stddef.h>

#include "common.h"

/* This array represents the 8 W5500 sockets */
extern struct W5500 this;

struct socket* W5500_socket(socket_n skt, socket_mode mode);

void W5500_socket_open(socket_n socket);
void W5500_socket_close(socket_n socket);
void W5500_socket_disconnect(socket_n socket);
void W5500_socket_listen(socket_n socket);
void W5500_socket_send(socket_n socket);
void W5500_socket_receive(socket_n socket, uint8_t *data, size_t data_size);
uint8_t W5500_socket_received_size(socket_n socket);
socket_sts W5500_socket_status(socket_n socket);

#endif /* socket_h_ */

/* [] END OF FILE */
