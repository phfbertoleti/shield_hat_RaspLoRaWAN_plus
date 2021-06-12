/* Header file com as definições da comunicação UART
 * Autor: Pedro Bertoleti 
 */
#ifndef COMM_UART_DEFS_H
#define COMM_UART_DEFS_H

/* Definições - caminho da UART e baudrate utilizado */
#define UART_TTGO_PATH        "/dev/serial0"
#define BAUDRATE_UART_TTGO    B115200

#endif

/* Protótipos */
int abre_serial(void);
int escreve_bytes(int fd, uint8_t * pt_buffer_escrita, size_t tamanho_buffer_escrita);
ssize_t le_bytes(int fd, uint8_t * pt_buffer_leitura, size_t tamanho_buffer_leitura);