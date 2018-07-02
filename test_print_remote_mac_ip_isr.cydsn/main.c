#include "project.h"

#include <stdio.h>
#include <stdbool.h>

#include "W5500.h"
#include "socket.h"
#include "tcp.h"
#include "hal_spi.h"

#include "UTILS.h"

void wiz_init(const uint8_t *ip_addr);
void socket0_init(void);

/* Interrupt handlers */
void handle_connection(void);
void handle_data(void);
void handle_disconnection(void);
void w5500_isr(void);

volatile bool w5500_interrupt_flag = false;

struct skt_t *tcp_server = NULL;
volatile uint8_t sir_sts = 0;

uint8_t data[1024] = {0};
char data_str[1024];

void send_data(void)
{
    uint16_t cnt = 0;
    uint16_t tx_ptr = 0;
    
    char headers[200];
    memset(headers, '\0', 100);
    
    const char *status_code = "HTTP/1.1 200 OK\n";
    const char *reply =
        "<!doctype html>\n"
        "<html><meta http-equiv=\"refresh\" content=\"10\"><body>\n"
        "<h1>Olduino 1802 BAGELS Server On Wiznet W5500</h1>\n"
        "</body></html>\n";
    
    sprintf(headers,
        "Content-Type: text/html; charset=UTF-8\n"
        "Content-Length: %d\r\n\r\n",
        strlen(reply));
    
    tx_ptr = W5500_socket_tx_write_ptr(tcp_server);
    W5500_socket_write(tcp_server, tx_ptr, (uint8_t *) status_code, strlen(status_code));
    tx_ptr += strlen(status_code);
    W5500_socket_tx_update_write_ptr(tcp_server, tx_ptr);
    W5500_socket_cmd(tcp_server, SOCKET_CMD_SEND);
    
    tx_ptr = W5500_socket_tx_write_ptr(tcp_server);
    W5500_socket_write(tcp_server, tx_ptr, (uint8_t *) headers, strlen(headers));
    tx_ptr += strlen(headers);
    W5500_socket_tx_update_write_ptr(tcp_server, tx_ptr);
    W5500_socket_cmd(tcp_server, SOCKET_CMD_SEND);
    
    tx_ptr = W5500_socket_tx_write_ptr(tcp_server);
    W5500_socket_write(tcp_server, tx_ptr, (uint8_t *) reply, strlen(reply));
    tx_ptr += strlen(reply);    
    W5500_socket_tx_update_write_ptr(tcp_server, tx_ptr);
    W5500_socket_cmd(tcp_server, SOCKET_CMD_SEND);
}

int main(void)
{
    memset(data_str, '\0', sizeof(data_str));
    const uint8_t ip_addr[] = {192, 168, 137, 70};
    
    isr_W5500_StartEx(w5500_isr);
    
    CyGlobalIntEnable;

    SPI_Start();
    UART_Start();
    
    /* "Clean" the serial port console */
    UART_PutChar(0x0C);
    
    /* Wait for the W5500 to start-up */
    CyDelay(100);
    
    wiz_soft_reset();
    
    /* W5500: Configure the MAC, IP, SUBNET and Gateway addresses */
    wiz_init(ip_addr);
    
    /* W5500: enable socket 0 interrupts */
    W5500_write_reg_byte(COMMON_REG_SOCKET_INTERRUPT_MASK, 0x01);

    /* Socket configuration, all others configurations are set to default */
    tcp_server = wiz_tcp_init_server(SOCKET_ID_0, 80);
    if (NULL == tcp_server) {
        UART_PutString("Error opening the socket\r\n");
        while(1);
    }
    
    W5500_socket_cmd(tcp_server, SOCKET_CMD_LISTEN);

    // Enable the CONN interrupt, disable all others first
    W5500_socket_set_imr(tcp_server, 0x00);
    W5500_socket_set_imr(tcp_server, 0x01);
    
    UART_PutString("Waiting for connection...\r\n");
    
    while (1) {
        
        if (true == w5500_interrupt_flag) {
            w5500_interrupt_flag = false;
            
            // clearing socket 0 interrupt on the general interrupt register
            W5500_spi_write_byte(BLOCK_COMMON_REGISTER, COMMON_REG_SOCKET_INTERRUPT, 0x01);
            
            // aca teniamos retrazo en limpiar la interrupcion por el uart
        
            sir_sts = W5500_socket_get_interrupt(tcp_server);

            if (sir_sts & 0x01) { // CON interrupt flag
                // Clear the interrupt flag
                // TODO: Add in the socket functions
                W5500_spi_write_byte(1, SOCKET_REG_INTERRUPT, 0x01);
                
                handle_connection();
                
                // Enable the RECV and DISCON interrupts, but disable all others first
                W5500_spi_write_byte(1, SOCKET_REG_INTERRUPT_MASK, 0x00);
                W5500_spi_write_byte(1, SOCKET_REG_INTERRUPT_MASK, 0x04 | 0x02);
            }
            
            if (sir_sts & 0x02) { // DISCON interrupt flag
                // Clear the interrupt flag
                W5500_spi_write_byte(1, SOCKET_REG_INTERRUPT, 0x02);
                
                handle_disconnection();
                
                // Enable the CON interrupt, but disable all others first
                W5500_spi_write_byte(1, SOCKET_REG_INTERRUPT_MASK, 0x00);
                W5500_spi_write_byte(1, SOCKET_REG_INTERRUPT_MASK, 0x01);
            }

            if (sir_sts & 0x04) { // RECV interrupt flag
                // Clear the interrupt flag
                W5500_spi_write_byte(1, SOCKET_REG_INTERRUPT, 0x04);
                
                handle_data();
                
                // Enable the SEND_OK RECV and DISCON interrupts, but disable all others first
                W5500_spi_write_byte(1, SOCKET_REG_INTERRUPT_MASK, 0x00);
                W5500_spi_write_byte(1, SOCKET_REG_INTERRUPT_MASK, 0x10 | 0x04 | 0x02);
                
                send_data();
            }
            
            if (sir_sts & 0x10) { // SEND interrupt flag
                // Clear the interrupt flag
                W5500_spi_write_byte(1, SOCKET_REG_INTERRUPT, 0x10);
                
                // Enable the RECV and DISCON interrupts, but disable all others first
                W5500_spi_write_byte(1, SOCKET_REG_INTERRUPT_MASK, 0x00);
                W5500_spi_write_byte(1, SOCKET_REG_INTERRUPT_MASK, 0x04 | 0x02);
                
                UART_PutString("Data sent\r\n");
                /* data back to normal */
                memset(data_str, '\0', sizeof(data_str));
                memset(data, 0, sizeof(data));
                
                /*
                UART_PutString("Closing the socket\r\n");
                W5500_socket_cmd_disconnect(tcp_server);
                W5500_socket_cmd_open(tcp_server);
                W5500_socket_cmd_listen(tcp_server);
                UART_PutString("Socket open and listening\r\n");
                */
            }
        }
        
    }
}

void wiz_init(const uint8_t *ip_addr)
{
    const uint8_t mac_addr[] = {0x00, 0xDE, 0xAD, 0xBE, 0xEF, 0x00};
    const uint8_t submask[] = {255, 255, 255, 0};
    const uint8_t gtw_addr[] = {192, 168, 1, 70}; // pc addr
        
    // SHAR (Source Hardware Address Register)
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x0009, mac_addr, sizeof(mac_addr));
    // gateway address register
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x0001, gtw_addr, sizeof(gtw_addr));
    // submask address
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x0005, submask, sizeof(submask));
    // ip address
    W5500_spi_write(BLOCK_COMMON_REGISTER, 0x000F, ip_addr, sizeof(ip_addr));
}

void handle_connection(void)
{
    UART_PutString("Remote connection...\r\n");
}

void handle_disconnection(void)
{
    UART_PutString("Closing the socket\r\n");
    W5500_socket_cmd(tcp_server, SOCKET_CMD_DISCONNECT);
    W5500_socket_cmd(tcp_server, SOCKET_CMD_OPEN);
    W5500_socket_cmd(tcp_server, SOCKET_CMD_LISTEN);
    UART_PutString("Socket open and listening\r\n");
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
    uint16_t rcv_size = 0;
    uint16_t data_addr = 0;
    
    LED_Write(~LED_Read());
        
    rcv_size = W5500_socket_rx_rcv_size(tcp_server);
    
    if (0 < rcv_size) {

        data_addr = W5500_socket_rx_read_ptr(tcp_server);
        W5500_socket_read(tcp_server, data_addr, data, rcv_size);
            
        /* Despues de leer los datos tenemos que actualizar Sn_RX_RD con el
        * tamaño de los datos que leimos. Mandar el comando RECV para notificar
        * al W5500. */
        data_addr += rcv_size;
        W5500_socket_rx_update_read_ptr(tcp_server, data_addr);
        
        data[rcv_size] = '\0';
        snprintf(data_str, sizeof(data_str), "%s", data);
    }
}

/* [] END OF FILE */
