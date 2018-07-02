#include "tcp.h"
#include "socket.h"

struct skt_t *wiz_tcp_init_server(socket_id socket, uint16_t port)
{
    struct skt_t *tcp_server = NULL;
    
    tcp_server = W5500_socket_init(socket, SOCKET_PROTOCOL_TCP,
                                   SOCKET_MEM_16KB, SOCKET_MEM_16KB);
    W5500_socket_set_port(tcp_server, port);
    W5500_socket_cmd(tcp_server, SOCKET_CMD_OPEN);
    
    return tcp_server;
}

/* [] END OF FILE */
