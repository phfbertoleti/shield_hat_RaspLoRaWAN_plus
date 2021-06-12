/* Header file com as definições de tratamento e envio de dados 
* Autor: Pedro Bertoleti 
*/
#ifndef TRATA_DADOS_RECEBIDOS_DEFS_H
#define TRATA_DADOS_RECEBIDOS_DEFS_H

#endif

/* Protótipos */
bool maquina_estados_recepcao(uint8_t byte_recebido);
int envia_dados_esp32(uint8_t * pt_dados, char tipo_de_dado, size_t tamanho);
int envia_linha_display_oled(uint8_t * pt_dados, char tipo_de_dado, size_t tamanho);
int trata_dados_recebidos(void);