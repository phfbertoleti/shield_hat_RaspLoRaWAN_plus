/* Módulo: definições da serial de configuração */

/* Header file com as definições da serial de comunicação com a Raspberry Pi */
#ifndef SERIAL_RASPPI_DEFS_H
#define SERIAL_RASPPI_DEFS_H

/* Definições de comunicação */
#define SERIAL_RASPPI_BAUDRATE          115200
#define SERIAL_RASPPI_BITS_E_PARIDADE   SERIAL_8N1
#define SERIAL_RASPPI_RX                17
#define SERIAL_RASPPI_TX                23

/* Buffer de recepção da serial de config */
#define BUFFER_RX_SERIAL_CONFIG_MAX_SIZE       300
char buffer_rx_serial_rasppi[BUFFER_RX_SERIAL_CONFIG_MAX_SIZE] = {0};
char * pt_buffer_rx_serial_rasppi = NULL;
int bytes_recebidos_buffer_rx_rasppi = 0;

#endif
