#include "project.h"

#include "W5500.h"
#include "hal_spi.h"

#include "socket.h"

void wiz_init(uint8_t *ip_addr);
void socket_init(void);
void handle_session(void);
void close_socket0(void);

extern struct W5500 this;

int main(void)
{
    uint8_t socket_status = 0;
    uint8_t old_socket_status = 0;
    // uint8_t ip_addr[] = {192, 168, 1, 90}; // 192, 168, 0, 182
    uint8_t ip_addr[] = {192, 168, 137, 70}; // 192, 168, 0, 182
    
    CyGlobalIntEnable;

    SPI_Start();
    UART_Start();
    UART_PutChar(0x0C);
    
    CyDelay(100);
    
    // reset
    uint8_t reset_cmd = 0x80;
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x0000, &reset_cmd, 1);
    
    wiz_init(ip_addr);
    
    // Test
    struct socket *skt_1 = W5500_socket(SOCKET_1, SOCKET_MODE_TCP);
    
    socket_init();
    
    while (1) {
        #if 0
        /* Wait for connection */
        do {
            W5500_spi_read(BLOCK_SOCKET_0_REGISTER, /* socket status register */ 0x0003, &socket_status, 1);
            CyDelay(100);
        } while(SOCKET_STS_LISTEN == socket_status);
        LED_Write(1);
        
        #else
        W5500_spi_read(BLOCK_SOCKET_0_REGISTER, /* socket status register */ 0x0003, &socket_status, 1);
        UART_PutString("Socket 0 Status = 0x");
        UART_PutHexByte(socket_status);
        UART_PutCRLF();
            
        switch (socket_status) {
        case SOCKET_STS_LISTEN:
            /* Socket is operating as TCP server mode and waiting for
             * connection-request (SYN packet) from a peer (TCP Client)
             * It will change to SOCKET_STABLISHED when the connection
             * request is successfully accepted.
             * Otherwise it will change to SOCK_CLOSED after TCPTO
             * occured (Sn_IR(TIMEOUT) = '1')
             */
            UART_PutString("Socket listening\r\n");
            break;
        case SOCKET_STS_CLOSED:
            UART_PutString("Opening the socket\r\n");
            socket_init();
            break;
        case SOCKET_STS_ESTABLISHED:
            UART_PutString("Handling session\r\n");
            handle_session();
            break;
        case SOCKET_STS_LAST_ACK:
        case SOCKET_STS_CLOSING:
        case SOCKET_STS_CLOSE_WAIT:
        case SOCKET_STS_FIN_WAIT:
        case SOCKET_STS_TIME_WAIT:
            UART_PutString("Closing the socket\r\n");
            close_socket0();
            socket_init();
            break;
        default:
            break;
        }
        
        CyDelay(250);
        #endif
    }
}

void wiz_init(uint8_t *ip_addr)
{
    const uint8_t mac_addr[] = {0x00, 0xDE, 0xAD, 0xBE, 0xEF, 0x00};
    const uint8_t submask[] = {255, 255, 255, 0};
    // const uint8_t gtw_addr[] = {192, 168, 1, 254}; // router addr
    const uint8_t gtw_addr[] = {192, 168, 1, 70}; // pc addr
    uint8_t bsz0 = 0;
    uint8_t bsz2 = 2;
    uint8_t bsz4 = 4;
    
    // set receive buffer for socket 0 to 4k
    W5500_spi_write(BLOCK_SOCKET_0_REGISTER, 0x001E, &bsz4, 1);
    // set transmit buffer for socket 0 to 4k
    W5500_spi_write(BLOCK_SOCKET_0_REGISTER, 0x001F, &bsz4, 1);
    // set receive buffer for socket 7 to 0k
    W5500_spi_write(BLOCK_SOCKET_7_REGISTER, 0x001E, &bsz0, 1);
    // set transmit buffer for socket 7 to 0k
    W5500_spi_write(BLOCK_SOCKET_7_REGISTER, 0x001E, &bsz0, 1);
    
    // SHAR (Source Hardware Address Register)
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x0009, mac_addr, sizeof(mac_addr));
    // gateway address register
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x0001, gtw_addr, sizeof(gtw_addr));
    // submask address
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x0005, submask, sizeof(submask));
    // ip address
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x000F, ip_addr, sizeof(ip_addr));
}

void socket_init(void)
{
    uint8_t tcpmode = 0x01;
    uint8_t cropen = 0x01;
    uint8_t crlisten = 0x02;
    uint8_t tcp_port[] = {0, 80}; // 80d = 50h
    
    // set mode register to TCP mode
    W5500_spi_write(BLOCK_SOCKET_0_REGISTER, 0x0000, &tcpmode, 1);
    // set tcp port to 80
    W5500_spi_write(BLOCK_SOCKET_0_REGISTER, 0x0004, tcp_port, sizeof(tcp_port));
    // Command: open socket
    W5500_spi_write(BLOCK_SOCKET_0_REGISTER, 0x0001, &cropen, 1);
    // leer el registro SR y comprobar que se abrio en modo tcp (0x13)
    // si se abrio en modo tcp entonces mandamos el comando de listen
    W5500_spi_read(BLOCK_SOCKET_0_REGISTER, 0x0000, &cropen, 1);
    UART_PutString("Socket 0 Mode Register = 0x");
    UART_PutHexByte(cropen);
    UART_PutCRLF();
    if (0 == cropen) {
        LED_Write(1);
        while(1);
    }
    
    CyDelay(10);
    // Command: listen to socket
    W5500_spi_write(BLOCK_SOCKET_0_REGISTER, 0x0001, &crlisten, 1);
}

void handle_session(void)
{
    LED_Write(1);
}

void close_socket0(void)
{
    LED_Write(0);
}

/* [] END OF FILE */
