/* Header file com as definições de recepção */
#ifndef RECEPCAO_DEFS_H
#define RECEPCAO_DEFS_H

/* Definições - bytes de sincronia */
#define BYTE_SINCRONIA_1         'R'
#define BYTE_SINCRONIA_2         'a'
#define BYTE_SINCRONIA_3         's'
#define BYTE_SINCRONIA_4         'p'
#define BYTE_SINCRONIA_5         '0'
#define BYTE_SINCRONIA_6         '1'

/* Definição - tamanho máximo dos dados a serem enviados ou recebidos
   (sem contar o header) */
#define TAMANHO_MAXIMO_DADOS                         255   

/* Definições - tipos de dados */
#define TIPO_DADO_LORAWAN_RECEBIDO_OU_ESCRITO        0x00
#define TIPO_DADO_ESCRITA_DAC                        0x01
#define TIPO_DADO_LEITURA_ADC                        0x02
#define TIPO_DADO_OLED_LINHA_1                       0x03
#define TIPO_DADO_OLED_LINHA_2                       0x04
#define TIPO_DADO_OLED_LINHA_3                       0x05
#define TIPO_DADO_OLED_LINHA_4                       0x06
#define TIPO_DADO_OLED_LINHA_5                       0x07
#define TIPO_DADO_OLED_LINHA_6                       0x08
#define TOTAL_TIPO_DADOS                             9

/* Definição - header utilizado na comunicação */
typedef struct __attribute__((__packed__))   
{
    char byte_sincronia_1;
    char byte_sincronia_2;
    char byte_sincronia_3;
    char byte_sincronia_4;
    char byte_sincronia_5;
    char byte_sincronia_6;
    char tipo_de_dados;
    unsigned char tamanho;
}THeader_dados;

#endif
