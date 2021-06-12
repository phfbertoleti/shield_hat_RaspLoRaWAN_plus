/* Módulo: tratamento e envio de dados via UART para Heltec ESP32 WiFi LoRa
 * Autor: Pedro Bertoleti 
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <MQTTClient.h>
#include "trata_e_envia_dados_lora.h"
#include "maquina_estados_defs.h"
#include "recepcao_defs.h"
#include "comunicacao_uart.h"
#include "debug_defs.h"
#include "dados_defs.h"
#include "mqtt_defs.h"

/* Variável com header dos dados recebidos */
THeader_dados header_dados;

/* File descriptor da UART */
extern int fd;

/* Variável para controlar o client MQTT */
extern MQTTClient client;

/* Array de dados recebidos */
unsigned char array_dados[TAMANHO_ARRAY_DADOS] = {0};

/* Ponteiro para o array de dados */
unsigned char * pt_dados = NULL;

/* Total de bytes a receber */
unsigned char tamanho_a_receber = 0;

/* Array com todos tipos de dados permitidos (recepção e envio) */
const char tipos_de_dados[TOTAL_TIPO_DADOS] = { TIPO_DADO_LORAWAN_RECEBIDO_OU_ESCRITO,
                                                TIPO_DADO_ESCRITA_DAC,
                                                TIPO_DADO_LEITURA_ADC,
                                                TIPO_DADO_OLED_LINHA_1,
                                                TIPO_DADO_OLED_LINHA_2,
                                                TIPO_DADO_OLED_LINHA_3,
                                                TIPO_DADO_OLED_LINHA_4,
                                                TIPO_DADO_OLED_LINHA_5,
                                                TIPO_DADO_OLED_LINHA_6 };

/* Protótipos locais */
char calcula_checksum(unsigned char * pt_dados, int qtde_bytes);
int verifica_se_tipo_de_dados_eh_valido(char tipo_recebido); 

/* Função: calcula o checksum dos dados a serem enviados
 * Parâmetros: ponteiro para os dados e quantidade de bytes para considerar no cálculo
 * Retorno: checksum calculado
 */
char calcula_checksum(unsigned char * pt_dados, int qtde_bytes)
{
    char soma_bytes = 0;
    char cks_calculado;
    int i;

    for (i=0; i<qtde_bytes; i++)
    {
        soma_bytes = soma_bytes + *pt_dados;
        pt_dados++;
    }

    cks_calculado = (~soma_bytes) + 1;
    return cks_calculado;
}

/* Função: verifica se o tipo de dados recebido é válido ou não
 * Parâmetros: tipo de dados recebido
 * Retorno: 0: válido
 *          -1: inválido
 */
int verifica_se_tipo_de_dados_eh_valido(char tipo_recebido)
{
    int resultado = -1;
    int i;
    
    for(i=0; i<TOTAL_TIPO_DADOS; i++)
    {
        if ( tipo_recebido == tipos_de_dados[i])
        {
            resultado = 0;
            break;
        }
    }

    return resultado;
}

/* Função: máquina de estados de recepcao de pacotes LoRa
 * Parâmetros: byte recebido
 * Retorno: true: recepção de um pacote completo
 *          false: recepção de pacote está incompleta
 */
bool maquina_estados_recepcao(uint8_t byte_recebido)
{
    bool recepcao_completa = false;
    char checksum_recebido = 0x00;

    switch (estado_recepcao)
    {
        case ESTADO_BYTE_SINCRONIA_1:
            if (byte_recebido == BYTE_SINCRONIA_1)
            {
                header_dados.byte_sincronia_1 = byte_recebido;
                estado_recepcao = ESTADO_BYTE_SINCRONIA_2;
            }
            break;

        case ESTADO_BYTE_SINCRONIA_2:
            if (byte_recebido == BYTE_SINCRONIA_2)
            {
                header_dados.byte_sincronia_2 = byte_recebido;
                estado_recepcao = ESTADO_BYTE_SINCRONIA_3;
            }
            else
                estado_recepcao = ESTADO_BYTE_SINCRONIA_1;
            break;

        case ESTADO_BYTE_SINCRONIA_3:
            if (byte_recebido == BYTE_SINCRONIA_3)
            {
                header_dados.byte_sincronia_3 = byte_recebido;
                estado_recepcao = ESTADO_BYTE_SINCRONIA_4;
            }
            else
                estado_recepcao = ESTADO_BYTE_SINCRONIA_1;

            break;

        case ESTADO_BYTE_SINCRONIA_4:
            if (byte_recebido == BYTE_SINCRONIA_4)
            {
                header_dados.byte_sincronia_4 = byte_recebido;
                estado_recepcao = ESTADO_BYTE_SINCRONIA_5;
            }
            else
                estado_recepcao = ESTADO_BYTE_SINCRONIA_1;

            break;

        case ESTADO_BYTE_SINCRONIA_5:
            if (byte_recebido == BYTE_SINCRONIA_5)
            {
                header_dados.byte_sincronia_5 = byte_recebido;
                estado_recepcao = ESTADO_BYTE_SINCRONIA_6;
            }
            else
                estado_recepcao = ESTADO_BYTE_SINCRONIA_1;

            break;

        case ESTADO_BYTE_SINCRONIA_6:
            if (byte_recebido == BYTE_SINCRONIA_6)
            {
                header_dados.byte_sincronia_4 = byte_recebido;
                estado_recepcao = ESTADO_RECEBE_TIPO_DE_DADOS;
            }
            else
                estado_recepcao = ESTADO_BYTE_SINCRONIA_1;

            break;
            
        case ESTADO_RECEBE_TIPO_DE_DADOS:
             if (verifica_se_tipo_de_dados_eh_valido(byte_recebido) == 0)
             {
                header_dados.tipo_de_dados = byte_recebido;
                estado_recepcao = ESTADO_RECEBE_TAMANHO;
             }
            else
                estado_recepcao = ESTADO_BYTE_SINCRONIA_1;
            break;

        case ESTADO_RECEBE_TAMANHO:
            header_dados.tamanho = (unsigned char)byte_recebido;
            tamanho_a_receber = (unsigned char)byte_recebido;
            pt_dados = array_dados;
            estado_recepcao = ESTADO_RECEBE_DADOS;
            break;   

        case ESTADO_RECEBE_DADOS:
            if (tamanho_a_receber > 0)
            {
                *pt_dados = byte_recebido;
                pt_dados++;         
                tamanho_a_receber--;                                       
            }

            if (tamanho_a_receber == 0)
            {
                estado_recepcao = ESTADO_AGUARDA_DADOS_VERIFICA_CKS;
            }

            break;

        case ESTADO_AGUARDA_DADOS_VERIFICA_CKS:
            checksum_recebido = byte_recebido;

            /* Verifica a integridade dos dados recebidos. Se o checksum
               estiver ok, sinaliza que a recepção está completa. 
            */
            if (checksum_recebido == calcula_checksum(array_dados, (int)header_dados.tamanho) )
            {
                #ifdef HABILITA_DEBUG_VIA_PRINTF
                printf("\r\nPacote recebido. Cks ok!\r\n");
                #endif

                recepcao_completa = true;    
            }
            else
            {
                #ifdef HABILITA_DEBUG_VIA_PRINTF
                printf("\r\nPacote recebido. Cks falhou\r\n");
                #endif

                recepcao_completa = false;
            }

            estado_recepcao = ESTADO_BYTE_SINCRONIA_1;

            break;
    }

    return recepcao_completa;
}

/* Função: tratamento de dados recebidos. 
 * Parâmetros: nenhum
 * Retorno: 0: tratamento feito com sucesso
 *         -1: falha ao tratar dados
 */
int trata_dados_recebidos(void)
{
    int status_tratamento = -1;

    switch (header_dados.tipo_de_dados)
    {
        case TIPO_DADO_LORAWAN_RECEBIDO_OU_ESCRITO:
            /* Tipo de dado recebido: dados vindos do LoRaWAN (mensagem de downlink) */
            #ifdef HABILITA_DEBUG_VIA_PRINTF
            printf("\r\nTipo de dado chegou via UART: TIPO_DADO_LORAWAN_RECEBIDO_OU_ESCRITO\r\n");
            #endif

            publish(client, MQTT_PUB_DADOS_LORAWAN_DOWNLINK_PARA_RASPPI, (char *)array_dados); 
            status_tratamento = 0;
            break;

        case TIPO_DADO_LEITURA_ADC:
            /* Tipo de dado recebido: leitura de ADC vinda do ESP32 */
            #ifdef HABILITA_DEBUG_VIA_PRINTF
            printf("\r\nTipo de dado chegou via UART: TIPO_DADO_LEITURA_ADC\r\n");
            #endif

            publish(client, MQTT_PUB_ADC_RASPPI, (char *)array_dados); 
            status_tratamento = 0;
            break;

        case TIPO_DADO_ESCRITA_DAC:
        default:
            /* Se for recebido um tipo de dado inválido ou se for um dado
               não esperado, nada é feito. */
            break;
    }    

    return status_tratamento;
}

/* Função: envia mensagem a ser escrita no OLED do ESP32 (com header e checksum)
 * Parâmetros: - ponteiro para dados
 *             - tipo de dado
 *             - tamanho dos dados
 * Retorno: 0: tratamento feito com sucesso
 *          Qualquer outra coisa: falha ao tratar dados
 */
int envia_linha_display_oled(uint8_t * pt_dados, char tipo_de_dado, size_t tamanho)
{
    THeader_dados header_dados;
    uint8_t checksum_calculado = 0x00;
    int status_envio;
    uint8_t array_dados_uart[TAMANHO_ARRAY_DADOS];
    
    if (tamanho > 20)
        tamanho = 20;

    memcpy(array_dados_uart, pt_dados, tamanho);

    /* Preenche header e calcula checksum */
    header_dados.byte_sincronia_1 = BYTE_SINCRONIA_1;
    header_dados.byte_sincronia_2 = BYTE_SINCRONIA_2;
    header_dados.byte_sincronia_3 = BYTE_SINCRONIA_3;
    header_dados.byte_sincronia_4 = BYTE_SINCRONIA_4;
    header_dados.byte_sincronia_5 = BYTE_SINCRONIA_5;
    header_dados.byte_sincronia_6 = BYTE_SINCRONIA_6;
    header_dados.tipo_de_dados = tipo_de_dado;
    header_dados.tamanho = (char)tamanho;
    checksum_calculado = (uint8_t)calcula_checksum(array_dados_uart, tamanho);

    /* Envia header, dados e checksum */
    status_envio = escreve_bytes(fd, (uint8_t *)&header_dados, sizeof(THeader_dados));
    status_envio = status_envio + escreve_bytes(fd, array_dados_uart, tamanho);
    status_envio = status_envio + escreve_bytes(fd, &checksum_calculado, 1);

    #ifdef HABILITA_DEBUG_VIA_PRINTF
    printf("\r\nDados escritos na UART para o ESP32 (tamanho: %d): \r\n", (int)tamanho);

    for(int i=0; i<tamanho; i++)
        printf("\r\nByte %d: %02X ", i, array_dados_uart[i]);

    printf("\r\n\n");
    #endif

    return status_envio;
}

/* Função: envia dados (com header e checksum) para Heltec ESP32 WiFi LoRa
 * Parâmetros: - ponteiro para dados
 *             - tipo de dado
 *             - tamanho dos dados
 * Retorno: 0: tratamento feito com sucesso
 *          Qualquer outra coisa: falha ao tratar dados
 */
int envia_dados_esp32(uint8_t * pt_dados, char tipo_de_dado, size_t tamanho)
{
    THeader_dados header_dados;
    uint8_t checksum_calculado = 0x00;
    int status_envio;
    uint8_t array_dados_uart[TAMANHO_ARRAY_DADOS];

    memcpy(array_dados_uart, pt_dados, tamanho);

    /* Preenche header e calcula checksum */
    header_dados.byte_sincronia_1 = BYTE_SINCRONIA_1;
    header_dados.byte_sincronia_2 = BYTE_SINCRONIA_2;
    header_dados.byte_sincronia_3 = BYTE_SINCRONIA_3;
    header_dados.byte_sincronia_4 = BYTE_SINCRONIA_4;
    header_dados.byte_sincronia_5 = BYTE_SINCRONIA_5;
    header_dados.byte_sincronia_6 = BYTE_SINCRONIA_6;
    header_dados.tipo_de_dados = tipo_de_dado;
    header_dados.tamanho = (char)tamanho;
    checksum_calculado = (uint8_t)calcula_checksum(array_dados_uart, tamanho);

    /* Envia header, dados e checksum */
    status_envio = escreve_bytes(fd, (uint8_t *)&header_dados, sizeof(THeader_dados));
    status_envio = status_envio + escreve_bytes(fd, array_dados_uart, tamanho);
    status_envio = status_envio + escreve_bytes(fd, &checksum_calculado, 1);

    #ifdef HABILITA_DEBUG_VIA_PRINTF
    printf("\r\nDados escritos na UART para o ESP32 (tamanho: %d): \r\n", (int)tamanho);

    for(int i=0; i<tamanho; i++)
        printf("\r\nByte %d: %02X ", i, array_dados_uart[i]);

    printf("\r\n\n");
    #endif

    return status_envio;
}