/* Header file com as definições da maquina de estados de recepção 
* Autor: Pedro Bertoleti 
*/
#ifndef MAQ_ESTADOS_RECEPCAO_DEFS_H
#define MAQ_ESTADOS_RECEPCAO_DEFS_H

/* Definições - estados usados na máquina de estados
                de recepção de dados */
#define ESTADO_BYTE_SINCRONIA_1                0x00
#define ESTADO_BYTE_SINCRONIA_2                0x01
#define ESTADO_BYTE_SINCRONIA_3                0x02
#define ESTADO_BYTE_SINCRONIA_4                0x03
#define ESTADO_BYTE_SINCRONIA_5                0x04
#define ESTADO_BYTE_SINCRONIA_6                0x05
#define ESTADO_RECEBE_TIPO_DE_DADOS            0x06
#define ESTADO_RECEBE_TAMANHO                  0x07
#define ESTADO_RECEBE_DADOS                    0x08
#define ESTADO_AGUARDA_DADOS_VERIFICA_CKS      0x09

/* Variável que controla o estado da máquina de estados de 
   recepção de dados */
char estado_recepcao = ESTADO_BYTE_SINCRONIA_1;

#endif