/*  Serviço: recepcao e envio de dados para o ESP32 (Heltec ESP32 WiFi LoRa) e 
 *           IPC (via MQTT)
 *  Autor: Pedro Bertoleti
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <termios.h>
#include <unistd.h>
#include <errno.h> 
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <MQTTClient.h>
#include "comunicacao_uart.h"
#include "trata_e_envia_dados_lora.h"
#include "debug_defs.h"
#include "dados_defs.h"
#include "recepcao_defs.h"
#include "mqtt_defs.h"

/* File descriptor da UART */
int fd;

/* Ponteiro para array de dados a serem escritos na UART */
uint8_t * pt_dados_para_escrever_uart = NULL;

/* Variável para controlar o client MQTT */
MQTTClient client;

/* Protótipos locais */
int on_message(void *context, char *topic_name, int topic_length, MQTTClient_message *message);

/* Funcao: publicacao de mensagens MQTT
 * Parametros: cleinte MQTT, topico MQTT and payload
 * Retorno: nenhum
*/
void publish(MQTTClient client, char* topic, char* payload) 
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 0;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
}

/* Funcao: callback de mensagens MQTT recebidas e echo para o broker
 * Parametros: contexto, ponteiro para nome do topico da mensagem recebida, tamanho do nome do topico e mensagem recebida
 * Retorno : 1: sucesso (fixo / nao ha checagem de erro neste exemplo)
*/
int on_message(void *context, char *topic_name, int topic_length, MQTTClient_message *message) 
{
    char * payload = message->payload;
    int tamanho_payload = message->payloadlen;
    unsigned char dado_adc;
    int valor_dac = 0;
    char payload_dac_padding[4] = {0};

    /* Mostra a mensagem recebida */
    #ifdef HABILITA_DEBUG_VIA_PRINTF
    printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topic_name, payload);
    #endif

    /* 
     * Toma ação de acordo com tópico e mensagem recebido 
     */

    /* Envio de dados para ESP32 (para o posterior envio LoRaWAN) */
    if (strcmp(topic_name, MQTT_SUB_DADOS_LORAWAN_UPLINK_PARA_ESP) == 0)
    {
        /* Repassa dados recebidos ao ESP32 */
        envia_dados_esp32((uint8_t *)payload, TIPO_DADO_LORAWAN_RECEBIDO_OU_ESCRITO, tamanho_payload);
    }     
    
    /* Envio de valor do DAC a ser escrito no ESP32 */
    if (strcmp(topic_name, MQTT_SUB_DAC_ESP32) == 0)
    {
        valor_dac = atoi(payload);
        sprintf(payload_dac_padding, "%03d", valor_dac);

        #ifdef HABILITA_DEBUG_VIA_PRINTF
        printf("Valor a ser escrito no DAC: %d\n", valor_dac);
        #endif

        if ( (valor_dac >= 0) &&  (valor_dac <= 255) )
        {            
            envia_dados_esp32((uint8_t *)&payload_dac_padding, TIPO_DADO_ESCRITA_DAC, 4);
        }
    }  

    /* Requisita ao ESP32 a leitura dos 3 canais do ADC */
    if (strcmp(topic_name, MQTT_SUB_ADC_RASPPI) == 0)
    {
        dado_adc = (unsigned char)*payload;
        envia_dados_esp32((uint8_t *)&dado_adc, TIPO_DADO_LEITURA_ADC, 1);
    }    

    /* Envia linha (1..6) a ser escrita no display OLED do Heltec ESP32 WiFi LoRa */
    if (strcmp(topic_name, MQTT_SUB_OLED_LINHA_1) == 0)
    {
        envia_linha_display_oled((uint8_t *)payload, TIPO_DADO_OLED_LINHA_1, tamanho_payload);
    }

    if (strcmp(topic_name, MQTT_SUB_OLED_LINHA_2) == 0)
    {
        envia_linha_display_oled((uint8_t *)payload, TIPO_DADO_OLED_LINHA_2, tamanho_payload);
    }

    if (strcmp(topic_name, MQTT_SUB_OLED_LINHA_3) == 0)
    {
        envia_linha_display_oled((uint8_t *)payload, TIPO_DADO_OLED_LINHA_3, tamanho_payload);
    }

    if (strcmp(topic_name, MQTT_SUB_OLED_LINHA_4) == 0)
    {
        envia_linha_display_oled((uint8_t *)payload, TIPO_DADO_OLED_LINHA_4, tamanho_payload);
    }

    if (strcmp(topic_name, MQTT_SUB_OLED_LINHA_5) == 0)
    {
        envia_linha_display_oled((uint8_t *)payload, TIPO_DADO_OLED_LINHA_5, tamanho_payload);
    }

    if (strcmp(topic_name, MQTT_SUB_OLED_LINHA_6) == 0)
    {
        envia_linha_display_oled((uint8_t *)payload, TIPO_DADO_OLED_LINHA_6, tamanho_payload);
    }


    MQTTClient_freeMessage(&message);
    MQTTClient_free(topic_name);
    return 1;
}

int main (int argc, char *argv[])
{
    uint8_t byte_lido;    
    int rc;
    int qtde_bytes_lidos;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    /* Aguarda 30 segundos para iniciar serviço */
    system("sleep 30");

    /* Inicializacao do MQTT (conexao & subscribe) */
    MQTTClient_create(&client, MQTT_ADDRESS, MQTT_CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    rc = MQTTClient_connect(client, &conn_opts);

    if (rc != MQTTCLIENT_SUCCESS)
    {
        #ifdef HABILITA_DEBUG_VIA_PRINTF
        printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
        #endif

        goto erro;
    }

    /* Faz subscribe nos tópicos necessários (para IPC) */
    MQTTClient_subscribe(client, MQTT_SUB_DADOS_LORAWAN_UPLINK_PARA_ESP, 0);
    MQTTClient_subscribe(client, MQTT_SUB_DAC_ESP32, 0);
    MQTTClient_subscribe(client, MQTT_SUB_ADC_RASPPI, 0);
    MQTTClient_subscribe(client, MQTT_SUB_OLED_LINHA_1, 0);
    MQTTClient_subscribe(client, MQTT_SUB_OLED_LINHA_2, 0);
    MQTTClient_subscribe(client, MQTT_SUB_OLED_LINHA_3, 0);
    MQTTClient_subscribe(client, MQTT_SUB_OLED_LINHA_4, 0);
    MQTTClient_subscribe(client, MQTT_SUB_OLED_LINHA_5, 0);
    MQTTClient_subscribe(client, MQTT_SUB_OLED_LINHA_6, 0);

    /* Abre comunicação com a porta serial */
    fd = abre_serial();
    if (fd <= 0) 
    { 
        #ifdef HABILITA_DEBUG_VIA_PRINTF
        printf("\r\n[ERRO] Impossivel abrir comunicacao com a serial.\r\n"); 
        #endif

        goto erro; 
    }

    while(1)
    {
        /* Leitura de dados da UART: le byte a byte da UART e 
           direciona cada byte à máquina de estados de recepção
           para posterior verififcação de integridade e tratamento
        */
        qtde_bytes_lidos = le_bytes(fd, &byte_lido, 1);
        if (qtde_bytes_lidos > 0)
        {
            if ( maquina_estados_recepcao(byte_lido) )
            {
                trata_dados_recebidos();
            }
        }
    }

    close(fd);
    return 0;      

erro:
    return -1;    
}
