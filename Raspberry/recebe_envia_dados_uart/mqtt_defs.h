/* Header file com as definições de MQTT do projeto 
* Autor: Pedro Bertoleti 
*/
#ifndef MQTT_DEFS_H
#define MQTT_DEFS_H

/* Definições - endereço do Broker e ID MQTT */
#define MQTT_ADDRESS        "tcp://127.0.0.1"
#define MQTT_CLIENTID       "mqtt_id_recebe_dados_uart"  

/* Topicos para direcionar dados recebidos do Heltec ESP32 WiFi LoRa
   (para IPC com demais aplicações) */
#define MQTT_PUB_DADOS_LORAWAN_DOWNLINK_PARA_RASPPI       "mqtt_lorawan_downlink_esp32"   //topico para receber do ESP32 mensagens de downlink recebidas
#define MQTT_PUB_ADC_RASPPI                               "mqtt_adc_para_rasppi"          //topico para receber a leitura do adc vinda do esp32

/* Topicos para solicitar dados ou operações ao Heltec ESP32 WiFi LoRa
   (para IPC com demais aplicações) */
#define MQTT_SUB_DADOS_LORAWAN_UPLINK_PARA_ESP            "mqtt_lorawan_uplink_esp32"     //topico para enviar dados a serem repassados via LoRaWAN (uplink)
#define MQTT_SUB_DAC_ESP32                                "mqtt_dac_para_esp32"           //topico para enviar ao esp32 valor para escrever no DAC
#define MQTT_SUB_ADC_RASPPI                               "mqtt_adc_para_esp32"           //topico para solicitar uma leitura de adc ao esp32
#define MQTT_SUB_OLED_LINHA_1                             "mqtt_oled_linha1_esp32"        //topico para enviar mensagem a ser escrita na linha 1 do OLED do ESP32
#define MQTT_SUB_OLED_LINHA_2                             "mqtt_oled_linha2_esp32"        //topico para enviar mensagem a ser escrita na linha 2 do OLED do ESP32
#define MQTT_SUB_OLED_LINHA_3                             "mqtt_oled_linha3_esp32"        //topico para enviar mensagem a ser escrita na linha 3 do OLED do ESP32
#define MQTT_SUB_OLED_LINHA_4                             "mqtt_oled_linha4_esp32"        //topico para enviar mensagem a ser escrita na linha 4 do OLED do ESP32
#define MQTT_SUB_OLED_LINHA_5                             "mqtt_oled_linha5_esp32"        //topico para enviar mensagem a ser escrita na linha 5 do OLED do ESP32
#define MQTT_SUB_OLED_LINHA_6                             "mqtt_oled_linha6_esp32"        //topico para enviar mensagem a ser escrita na linha 6 do OLED do ESP32

#endif

/* Protótipos */
void publish(MQTTClient client, char* topic, char* payload);