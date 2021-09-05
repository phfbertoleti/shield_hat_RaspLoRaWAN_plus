/* Header file com as definições das filas */
#ifndef FILAS_DEFS_H
#define FILAS_DEFS_H

/* Definições dos tempos de espera de escritas e leituras das filas */
#define TICKS_ESPERA_ESCRITA_FILAS         ( TickType_t ) 1000
#define TICKS_ESPERA_LEITURA_FILAS         ( TickType_t ) 1000

/* Definições - tamanhos das filas */
#define TAMANHO_FILA_DADOS_LORAWAN_DOWNLINK         50
#define TAMANHO_FILA_DADOS_LORAWAN_UPLINK           10

/* Fila dos dados para enviar por LoRaWAN */
QueueHandle_t xQueue_dados_lorawan;

/* Fila dos dados de downlink do LoRaWAN */
QueueHandle_t xQueue_lorawan_downlink;

/* Fila de dados a serem escritos no display */
QueueHandle_t xQueue_display;


#endif
