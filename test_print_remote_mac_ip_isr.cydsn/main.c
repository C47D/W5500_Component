#include "project.h"
#include <stdio.h>
#include <stdbool.h>

#include "W5500.h"
#include "hal_spi.h"

void wiz_init(const uint8_t *ip_addr);
void socket0_init(void);
void close_socket0(void);

/* Interrupt handlers */
void handle_connection(void);
void handle_data(void);
void handle_disconnection(void);
void w5500_isr(void);

volatile bool w5500_interrupt_flag = false;
volatile uint8_t sir_sts = 0;

int main(void)
{
    const uint8_t ip_addr[] = {192, 168, 137, 70};
    
    isr_W5500_StartEx(w5500_isr);
    
    CyGlobalIntEnable;

    SPI_Start();
    UART_Start();
    UART_PutChar(0x0C);
    
    CyDelay(100);
    
    W5500_spi_write_byte(BLOCK_COMMON_REGISTER, SOCKET_REG_MODE, 0x80);
    
    wiz_init(ip_addr);
    
    socket0_init();
    UART_PutString("Waiting for connection...\r\n");
    
    while (1) {
        
        if (true == w5500_interrupt_flag) {
            w5500_interrupt_flag = false;
            UART_PutString("Socket 0 interrupt\r\n");
            
            // clearing socket 0 interrupt
            W5500_spi_write_byte(BLOCK_COMMON_REGISTER, 0x0017, 0x01);
        
            sir_sts = W5500_spi_read_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT);
            UART_PutString("Socket 0 Interrupt Register: 0x");
            UART_PutHexByte(sir_sts);
            UART_PutCRLF();
        
            W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT, sir_sts);
            
            if (sir_sts & 0x01) { // CON interrupt flag
                // Clear the interrupt flag
                W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT, 0x01);
                
                handle_connection();
                
                // Enable the RECV and DISCON interrupts, but disable all others first
                W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT_MASK, 0x00);
                W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT_MASK, 0x04 | 0x02);
            }
            
            if (sir_sts & 0x02) { // DISCON interrupt flag
                // Clear the interrupt flag
                W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT, 0x02);
                
                handle_disconnection();
                
                // Enable the CON interrupt, but disable all others first
                W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT_MASK, 0x00);
                W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT_MASK, 0x01);
            }

            if (sir_sts & 0x04) { // RECV interrupt flag
                // Clear the interrupt flag
                W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT, 0x04);
                
                handle_data();
                
                // Enable the RECV and DISCON interrupts, but disable all others first
                W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT_MASK, 0x00);
                W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT_MASK, 0x04 | 0x02);
            }
        }
        
    }
}

void wiz_init(const uint8_t *ip_addr)
{
    const uint8_t mac_addr[] = {0x00, 0xDE, 0xAD, 0xBE, 0xEF, 0x00};
    const uint8_t submask[] = {255, 255, 255, 0};
    const uint8_t gtw_addr[] = {192, 168, 1, 70}; // pc addr
    
    // set receive buffer for socket 0 to 1k
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_RX_BUFFER_SIZE, 1);
    // set transmit buffer for socket 0 to 1k
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_TX_BUFFER_SIZE, 2);
    
    // SHAR (Source Hardware Address Register)
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x0009, mac_addr, sizeof(mac_addr));
    // gateway address register
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x0001, gtw_addr, sizeof(gtw_addr));
    // submask address
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x0005, submask, sizeof(submask));
    // ip address
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x000F, ip_addr, sizeof(ip_addr));
    // enable socket 0 interrupts
    W5500_spi_write_byte(BLOCK_COMMON_REGISTER, 0x0018, 0x01);
}

void socket0_init(void)
{
    uint8_t tcp_port[] = {0, 80}; // 80d = 50h
    
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_MODE, SOCKET_MODE_TCP);
    // set tcp port to 80
    W5500_spi_write(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_SOURCE_PORT0, tcp_port, sizeof(tcp_port));
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_COMMAND, SOCKET_CMD_OPEN);
    
    CyDelay(10);
    
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_COMMAND, SOCKET_CMD_LISTEN);
    
    // Enable only the CON interrupt, but disable all others first
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT_MASK, 0x00);
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT_MASK, 0x01);
}

void handle_connection(void)
{
    uint8_t skt_remote_ip[4] = {0};
    uint8_t skt_remote_mac[6] = {0};
    char remote_mac[100];
    char remote_ip[50];
    
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
}

void close_socket0(void)
{
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_COMMAND, SOCKET_CMD_DISCONNECT);
    UART_PutString("Socket 0 closed\r\n");
}

void handle_disconnection(void)
{
    UART_PutString("Closing the socket\r\n");
    close_socket0();
    UART_PutString("Socket open and listening\r\n");
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_COMMAND, SOCKET_CMD_OPEN);
    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_COMMAND, SOCKET_CMD_LISTEN);
}

/* Socket Status Register:
 * El pin INTx se pone en 0 logico cuando ocurre una interrupcion que
 * esta habilitada, al leer el registro de los flags de las interrupciones
 * se puede ver el flag en 1 logico, pero además podemos tener mas bits
 * en 1 logico, sin embargo si no estan habilitados en el mask interrupts
 * estos flags no pueden causar que el INTx se vaya a 0 logico.
 */
void w5500_isr(void)
{
    w5500_interrupt_flag = true;
    W5500_ISR_ClearInterrupt();
}

void handle_data(void)
{
    uint8_t skt_rcv_data_size[2] = {0};
    uint8_t data_ptr[2] = {0};
    uint16_t bytes_rcvd = 0;
    uint16_t new_rx_addr = 0;
    uint16_t data_addr = 0;
    uint8_t data[100] = {0};
    char data_str[100];
    
    LED_Write(~LED_Read());

    W5500_spi_write_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_INTERRUPT, sir_sts);
    
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
            
        /* Read Sn_RX_RD (Socket n RX Read Pointer) to get RX Data Start Address */
        data_ptr[0] = W5500_spi_read_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_RX_READ_POINTER0);
        data_ptr[1] = W5500_spi_read_byte(BLOCK_SOCKET_0_REGISTER, SOCKET_REG_RX_READ_POINTER1);
        data_addr = (data_ptr[0] << 8 | data_ptr[1]);
            
        UART_PutString("Address of data: ");
        UART_PutHexInt(data_addr);
        UART_PutCRLF();
            
        /* Read received Data from Sn_RX_BUF */
        W5500_spi_read(BLOCK_SOCKET_0_RX_BUFFER, data_addr, data, sizeof(data));
            
        /* Despues de leer los datos tenemos que actualizar Sn_RX_RD con el
        * tamaño de los datos que leimos. Mandar el comando RECV para notificar
        * al W5500. */
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
}

/* [] END OF FILE */
