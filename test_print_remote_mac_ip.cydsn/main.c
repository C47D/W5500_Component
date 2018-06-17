#include "project.h"
#include <stdio.h>

#include "W5500.h"
#include "hal_spi.h"

void wiz_init(uint8_t *ip_addr);
void socket_init(void);
void handle_session(void);
void close_socket0(void);

int main(void)
{
    uint8_t socket_status = 0;
    uint8_t old_socket_status = 0;
    uint8_t ip_addr[] = {192, 168, 137, 70};
    
    CyGlobalIntEnable;

    SPI_Start();
    UART_Start();
    UART_PutChar(0x0C);
    
    CyDelay(100);
    
    W5500_spi_write_byte(BLOCK_COMMON_REGISTER, SOCKET_REG_MODE, 0x80);
    
    wiz_init(ip_addr);
    
    socket_init();
    
    while (1) {
        socket_status = W5500_spi_read_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_STATUS);
        
        if (socket_status != old_socket_status) {
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
        }
            
        old_socket_status = socket_status;
        
        CyDelay(500);
    }
}

void wiz_init(uint8_t *ip_addr)
{
    const uint8_t mac_addr[] = {0x00, 0xDE, 0xAD, 0xBE, 0xEF, 0x00};
    const uint8_t submask[] = {255, 255, 255, 0};
    const uint8_t gtw_addr[] = {192, 168, 1, 70}; // pc addr
    
    // set receive buffer for socket 0 to 4k
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_RX_BUFFER_SIZE, 4);
    // set transmit buffer for socket 0 to 4k
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_TX_BUFFER_SIZE, 4);
    
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
    uint8_t tcp_port[] = {0, 80}; // 80d = 50h
    
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_MODE, SOCKET_MODE_TCP);
    // set tcp port to 80
    W5500_spi_write(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_SOURCE_PORT0, tcp_port, sizeof(tcp_port));
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_COMMAND, SOCKET_CMD_OPEN);
    
    UART_PutString("Socket 0 Mode Register = 0x");
    UART_PutHexByte(W5500_spi_read_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_MODE));
    UART_PutCRLF();
    
    CyDelay(10);
    
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_COMMAND, SOCKET_CMD_LISTEN);
}

void handle_session(void)
{
    uint8_t skt_status = 0;
    uint8_t skt_remote_ip[4] = {0};
    uint8_t skt_remote_mac[6] = {0};
    uint8_t skt_rcv_data_size[2] = {0};
    char remote_mac[100];
    char remote_ip[50];
    uint8_t data_ptr[2] = {0};
    uint16_t bytes_rcvd = 0;
    uint16_t new_rx_addr = 0;
    uint16_t data_addr = 0;
    uint8_t data[100] = {0};
    char data_str[100];
    
    LED_Write(1);
    
    UART_PutString("Remote connection...\r\n");
    
    // Reading the remote MAC
    W5500_spi_read(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_DESTINATION_HW_ADDR0,
        skt_remote_mac, sizeof(skt_remote_mac));
    // Reading the remote IP Address
    W5500_spi_read(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_DESTINATION_IP_ADDR0,
        skt_remote_ip, sizeof(skt_remote_ip));
    
    snprintf(remote_mac, sizeof(remote_mac), "Remote MAC: %X %X %X %X %X %X\r\n", skt_remote_mac[0],
        skt_remote_mac[1], skt_remote_mac[2], skt_remote_mac[3], skt_remote_mac[4], skt_remote_mac[5]);
    
    snprintf(remote_ip, sizeof(remote_ip), "Remote IP: %d %d %d %d\r\n", skt_remote_ip[0],
        skt_remote_ip[1], skt_remote_ip[2], skt_remote_ip[3]);
    
    UART_PutString(remote_mac);
    UART_PutString(remote_ip);
    
    while (1) {
        
        skt_status = W5500_spi_read_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_STATUS);
        if (SOCKET_STS_ESTABLISHED != skt_status) {
            UART_PutString("Connection closed, exiting while loop\r\n");
            break;
        }
        
        // Read Sn_RX_RSR (Socket n Received Size), if it's > 0 then we have data
        // Se calcula apartir de la diferencia entre el Sn_RX_WR y Sn_RX_RD
        skt_rcv_data_size[0] = W5500_spi_read_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_RX_RECEIVED_SIZE0);
        skt_rcv_data_size[1] = W5500_spi_read_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_RX_RECEIVED_SIZE1);
        bytes_rcvd = (skt_rcv_data_size[0] << 8) | skt_rcv_data_size[1];
        
        if (0 < bytes_rcvd) {
            UART_PutString("Size of received data: ");
            UART_PutHexInt(bytes_rcvd);
            UART_PutCRLF();
            
            UART_PutString("Getting the received data\r\n");
            /* The socket n RX Buffer Block allocated in 16KB Rx memory is buffer for
             * saving the received data through the Ethernet. The 16bits Offset Address
             * of Socket n Rx Buffer Block has 64KB address space ranged from 0x0000
             * to 0xFFFF, and it is configured with reference to Socket n RX RD Pointer
             * Register (Sn_RX_RD) & Socket n RX Write Pointer Register (Sn_RX_WR).
             * However, the 16bits Offset Address automatically converts into the
             * physical address to be accesible in 16KB RX memory.
             */
            
            /* Read Sn_RX_RD (Socket n RX Read Pointer) to get RX Data Start Address
             */
            data_ptr[0] = W5500_spi_read_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_RX_READ_POINTER0);
            data_ptr[1] = W5500_spi_read_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_RX_READ_POINTER1);
            data_addr = (data_ptr[0] << 8 | data_ptr[1]);
            
            UART_PutString("Address of data: ");
            UART_PutHexInt(data_addr);
            UART_PutCRLF();
            
            /* Read received Data from Sn_RX_BUF
             */
            W5500_spi_read(BLOCK_SOCKET_0_RX_BUFFER, data_addr, data, sizeof(data));
            
            /* Despues de leer los datos tenemos que actualizar Sn_RX_RD con el
             * tamaño de los datos que leimos. Mandar el comando RECV para notificar
             * al W5500.
             */
            new_rx_addr = data_addr + bytes_rcvd;
            data_ptr[0] = (new_rx_addr & 0xFF00) >> 8;
            data_ptr[1] = new_rx_addr & 0xFF;
            UART_PutString("Bytes received: ");
            UART_PutHexInt(bytes_rcvd);
            UART_PutCRLF();            
            UART_PutString("New rx addr: ");
            UART_PutHexInt(new_rx_addr);
            UART_PutCRLF();                      
            W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_RX_READ_POINTER0, data_ptr[0]);
            W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_RX_READ_POINTER1, data_ptr[1]);
            
            W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_COMMAND, SOCKET_CMD_RECV);
            
            /* Add a NULL char so we can print the received data without problems */
            data[bytes_rcvd] = '\0';
            snprintf(data_str, sizeof(data_str), "data received: %s\r\n", data);
            UART_PutString(data_str);
            
            memset(data_str, '\0', sizeof(data_str));
            memset(data, 0, sizeof(data));
        }
        
        CyDelay(100);
    }
}

void close_socket0(void)
{
    LED_Write(0);
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_COMMAND, SOCKET_CMD_DISCONNECT);
}

/* [] END OF FILE */
