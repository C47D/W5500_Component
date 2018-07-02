#if !defined(TCP_H_)
#define TCP_H_

#include "socket.h"

struct skt_t *wiz_tcp_init_server(socket_id socket, uint16_t port);

#endif /* tcp_h_ */

/* [] END OF FILE */
