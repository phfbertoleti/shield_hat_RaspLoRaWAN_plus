/* Programa: unidade lorawan do projeto
 * Autor: Pedro Bertoleti
 */
#include <driver/adc.h>
#include <esp_adc_cal.h> 
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_task_wdt.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "FILAS_defs.h"
#include "RADIO_LORA_defs.h"
#include "recepcao_defs.h"
#include "maquina_estados_defs.h"
#include "LORAWAN_defs.h"
#include "PRIOS_defs.h"
#include "STACKS_SIZE_defs.h"
#include "serial_rasppi.h"
#include "SEMAFOROS_defs.h"
#include "DAC_defs.h"
#include "OLED_defs.h"
#include "imagens_defs.h"

/* Definição - tempo máximo sem alimentar o watchdog */
#define TEMPO_WATCHDOG_SEGUNDOS    60

/* Constantes do rádio LoRa: GPIOs utilizados para comunicação
   com rádio SX1276 */
const lmic_pinmap lmic_pins = {
  .nss = RADIO_NSS_PORT,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = RADIO_RESET_PORT,
  .dio = {RADIO_DIO_0_PORT, RADIO_DIO_1_PORT, LMIC_UNUSED_PIN},  //dio2 não é utilizado.
};

/* Objeto para envio e recepção LoRaWAN (ABP) */
static osjob_t sendjob;

/* Estrutura que contem as informacoes para calibracao do ADC */
esp_adc_cal_characteristics_t adc_cal;

/* Array com todos tipos de dados permitidos */
const char tipos_de_dados[TOTAL_TIPO_DADOS] = {TIPO_DADO_LORAWAN_RECEBIDO_OU_ESCRITO,
                                               TIPO_DADO_ESCRITA_DAC,
                                               TIPO_DADO_LEITURA_ADC,
                                               TIPO_DADO_OLED_LINHA_1,
                                               TIPO_DADO_OLED_LINHA_2,
                                               TIPO_DADO_OLED_LINHA_3,
                                               TIPO_DADO_OLED_LINHA_4,
                                               TIPO_DADO_OLED_LINHA_5,
                                               TIPO_DADO_OLED_LINHA_6 };

/* Variável com header dos dados recebidos */
THeader_dados header_dados;

/* Total de bytes a receber */
unsigned char tamanho_a_receber = 0;

/* Array de dados recebidos */
unsigned char array_dados[TAMANHO_MAXIMO_DADOS] = {0};

/* Ponteiro para o array de dados */
unsigned char * pt_dados = NULL;

/* Protótipos de funções auxiliares */
char calcula_checksum(unsigned char * pt_dados, int qtde_bytes);
int verifica_se_tipo_de_dados_eh_valido(char tipo_recebido);
bool maquina_estados_recepcao(uint8_t byte_recebido);
void escreve_serial_debug(char * ptr_string);
void do_send(osjob_t* j, unsigned long total, unsigned long parcial);
void configura_adc(void);

/* Prototipos das tarefas */
void task_oled( void *pvParameters );
void task_serial_rasppi( void *pvParameters );
void task_envio_lorawan( void *pvParameters );

/*
 * Implementações
 */
 
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
    char cks_calculado = 0x00;
    char linha_debug[100];

    switch (estado_recepcao)
    {
        case ESTADO_BYTE_SINCRONIA_1:
            escreve_serial_debug("Estado: ESTADO_BYTE_SINCRONIA_1");
            if (byte_recebido == BYTE_SINCRONIA_1)
            {
                header_dados.byte_sincronia_1 = byte_recebido;
                estado_recepcao = ESTADO_BYTE_SINCRONIA_2;
            }
            break;

        case ESTADO_BYTE_SINCRONIA_2:
            escreve_serial_debug("Estado: ESTADO_BYTE_SINCRONIA_2");
            if (byte_recebido == BYTE_SINCRONIA_2)
            {
                header_dados.byte_sincronia_2 = byte_recebido;
                estado_recepcao = ESTADO_BYTE_SINCRONIA_3;
            }
            else
                estado_recepcao = ESTADO_BYTE_SINCRONIA_1;
            break;

        case ESTADO_BYTE_SINCRONIA_3:
            escreve_serial_debug("Estado: ESTADO_BYTE_SINCRONIA_3");
            if (byte_recebido == BYTE_SINCRONIA_3)
            {
                header_dados.byte_sincronia_3 = byte_recebido;
                estado_recepcao = ESTADO_BYTE_SINCRONIA_4;
            }
            else
                estado_recepcao = ESTADO_BYTE_SINCRONIA_1;

            break;

        case ESTADO_BYTE_SINCRONIA_4:
            escreve_serial_debug("Estado: ESTADO_BYTE_SINCRONIA_4");
            if (byte_recebido == BYTE_SINCRONIA_4)
            {
                header_dados.byte_sincronia_4 = byte_recebido;
                estado_recepcao = ESTADO_BYTE_SINCRONIA_5;
            }
            else
                estado_recepcao = ESTADO_BYTE_SINCRONIA_1;

            break;

        case ESTADO_BYTE_SINCRONIA_5:
            escreve_serial_debug("Estado: ESTADO_BYTE_SINCRONIA_5");
            if (byte_recebido == BYTE_SINCRONIA_5)
            {
                header_dados.byte_sincronia_5 = byte_recebido;
                estado_recepcao = ESTADO_BYTE_SINCRONIA_6;
            }
            else
                estado_recepcao = ESTADO_BYTE_SINCRONIA_1;

            break;

        case ESTADO_BYTE_SINCRONIA_6:
            escreve_serial_debug("Estado: ESTADO_BYTE_SINCRONIA_6");
            if (byte_recebido == BYTE_SINCRONIA_6)
            {
                header_dados.byte_sincronia_4 = byte_recebido;
                estado_recepcao = ESTADO_RECEBE_TIPO_DE_DADOS;
            }
            else
                estado_recepcao = ESTADO_BYTE_SINCRONIA_1;

            break;
            
        case ESTADO_RECEBE_TIPO_DE_DADOS:
             escreve_serial_debug("Estado: ESTADO_RECEBE_TIPO_DE_DADOS");
             if (verifica_se_tipo_de_dados_eh_valido(byte_recebido) == 0)
             {
                header_dados.tipo_de_dados = byte_recebido;
                estado_recepcao = ESTADO_RECEBE_TAMANHO;
             }
            else
                estado_recepcao = ESTADO_BYTE_SINCRONIA_1;
            break;

        case ESTADO_RECEBE_TAMANHO:
            sprintf(linha_debug, "Estado: ESTADO_RECEBE_TAMANHO: %d", (int)byte_recebido);
            escreve_serial_debug(linha_debug);
            
            header_dados.tamanho = (unsigned char)byte_recebido;
            tamanho_a_receber = (unsigned char)byte_recebido;
            memset(array_dados, 0x00, sizeof(array_dados));
            pt_dados = array_dados;
            estado_recepcao = ESTADO_RECEBE_DADOS;
            break;   

        case ESTADO_RECEBE_DADOS:
            escreve_serial_debug("Estado: ESTADO_RECEBE_DADOS");
            
            if (tamanho_a_receber > 0)
            {
                escreve_serial_debug("Byte de dado recebido.");
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
            escreve_serial_debug("Estado: ESTADO_AGUARDA_DADOS_VERIFICA_CKS");
            checksum_recebido = byte_recebido;
            cks_calculado = calcula_checksum(array_dados, (int)header_dados.tamanho);

            if (checksum_recebido ==  cks_calculado)
            {
                escreve_serial_debug("Estado: ESTADO_AGUARDA_DADOS_VERIFICA_CKS: cks ok");                
                recepcao_completa = true;    
            }
            else
            {
                sprintf(linha_debug, "cks falhou! Esperado: %02X; Recebido: %02X", cks_calculado, checksum_recebido);
                escreve_serial_debug(linha_debug);
                recepcao_completa = false;
            }

            estado_recepcao = ESTADO_BYTE_SINCRONIA_1;

            break;
    }

    return recepcao_completa;
}

/*  Funcao: escreve na serial, considerando o semoforo
 *  Parometros: ponteiro para string a ser escrita
 *  Retorno: nenhum
 */
void escreve_serial_debug(char * ptr_string)
{
    if (xSemaphoreTake(xSerial_semaphore, TICKS_ESPERA_SEMAFORO_SERIAL ) == pdTRUE)
    {
        Serial.println(ptr_string);
        xSemaphoreGive(xSerial_semaphore);
    }
}

/* Callbacks para uso cpm OTAA apenas (por este projeto usar ABP, isso, eles 
 *  estão vazios) */
void os_getArtEui (u1_t* buf) 
{ 
    /* Não utilizado neste projeto */  
}

void os_getDevEui (u1_t* buf) 
{ 
    /* Não utilizado neste projeto */  
}

void os_getDevKey (u1_t* buf) 
{ 
    /* Não utilizado neste projeto */  
}

/* Callback de evento: todo evento do LoRaWAN irá chamar essa
   callback, de forma que seja possível saber o status da 
   comunicação com o gateway LoRaWAN. */
void onEvent (ev_t ev) 
{
    char cks_calculado;
    char array_downlink[TAMANHO_MAXIMO_DADOS+1] = {0};
    
    switch(ev) 
    {
        case EV_SCAN_TIMEOUT:
            break;
        case EV_BEACON_FOUND:
            break;
        case EV_BEACON_MISSED:
            break;
        case EV_BEACON_TRACKED:
            break;
        case EV_JOINING:
            break;
        case EV_JOINED:
            break;
        case EV_JOIN_FAILED:
            break;
        case EV_REJOIN_FAILED:
            break;
        case EV_TXCOMPLETE:
            escreve_serial_debug("EV_TXCOMPLETE (incluindo espera pelas janelas de recepção)");

            /* Verifica se ack foi recebido do gateway */
            if (LMIC.txrxFlags & TXRX_ACK)
              escreve_serial_debug("Ack recebido");

            /* Verifica se foram recebidos dados do gateway */  
            if (LMIC.dataLen > 0) 
            {
                escreve_serial_debug("Recebidos bytes (downlink)");
                array_downlink[0] = (char)LMIC.dataLen;
                memcpy(array_downlink+1, LMIC.frame + LMIC.dataBeg, (char)LMIC.dataLen);
                xQueueSend( xQueue_lorawan_downlink, (void *) &array_downlink, TICKS_ESPERA_ESCRITA_FILAS);
            }

            /* Devolve o controle do semáforo da serial */
            xSemaphoreGive(xSerial_semaphore);
            
            break;

        case EV_LOST_TSYNC:
            break;
        case EV_RESET:
            break;
        case EV_RXCOMPLETE:
            break;
        case EV_LINK_DEAD:
            break;
        case EV_LINK_ALIVE:
            break;
        case EV_TXSTART:            
            break;
        default:
            break;
    }
}

/* Função para envio de dados ao gateway LoRaWAN */
void do_send(osjob_t* j, unsigned char * pt_dados)
{
    static uint8_t mydata[TAMANHO_MAXIMO_DADOS+1];
    unsigned char tamanho_dados;

    esp_task_wdt_reset();

    /* Le tamanho dos dados e o copia para array mydata */
    tamanho_dados = *pt_dados;
    pt_dados++;
    memcpy(mydata, pt_dados, tamanho_dados);
    
    /* Verifica se já há um envio sendo feito.
       Em caso positivo, o envio atual é suspenso. */
    if (LMIC.opmode & OP_TXRXPEND) 
    {
        escreve_serial_debug("OP_TXRXPEND: ha um envio ja pendente, portanto o envio atual nao sera feito");
    } 
    else 
    {
        /* Aqui, o envio pode ser feito. */
        /* O pacote LoRaWAN é montado e o coloca na fila de envio. */
        LMIC_setTxData2(4, mydata, tamanho_dados, ENVIO_SEM_CONFIRMACAO);
        escreve_serial_debug("Pacote LoRaWAN na fila de envio."); 
    }
    
    esp_task_wdt_reset();
}

/* Função: configura ADC para leitura de tensão (tensao max: 3,9V)
 * Parâmetros: nenhum
 * Retorno:  nenhum
 */
void configura_adc(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_1,ADC_ATTEN_DB_11);
    adc1_config_channel_atten(ADC1_CHANNEL_2,ADC_ATTEN_DB_11);
    esp_adc_cal_value_t adc_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_cal);
}

void setup() 
{
    int tamanho_item_fila;

    /* Inicialização da serial de bebug */
    Serial.begin(115200);

    /* Inicializacao da serial de configuracao */
    Serial1.begin(SERIAL_RASPPI_BAUDRATE,
                  SERIAL_RASPPI_BITS_E_PARIDADE,
                  SERIAL_RASPPI_RX,
                  SERIAL_RASPPI_TX);

    /* Configura ADC para uso */
    configura_adc();

    /* Inicializa display */
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) 
    { 
        Serial.println("[ERRO] não foi possivel inicializar display. O ESP32 será reiniciado em 1s...");
        delay(1000);
        ESP.restart();
    }
    else
    {
        Serial.println("Display inicializado.");
        
        display.clearDisplay();
        display.drawBitmap(0, 0, lorawan_logo, 128, 64, WHITE);
        display.display();        
    }
    
    /* Criacao dos semaforos */
    xSerial_semaphore = xSemaphoreCreateMutex();
  
    if (xSerial_semaphore == NULL)
    {
        Serial.println("[ERRO] Nao e possivel criar um ou mais semaforos. ESP32 reiniciara em 1s...");
        delay(1000);
        ESP.restart();
    }

    /* Criacao das filas  */
    tamanho_item_fila = sizeof(THeader_dados) + TAMANHO_MAXIMO_DADOS + 1; //+1 devido ao byte de tamanho
    xQueue_dados_lorawan = xQueueCreate(TAMANHO_FILA_DADOS_LORAWAN_DOWNLINK, tamanho_item_fila);  
    xQueue_lorawan_downlink = xQueueCreate(TAMANHO_FILA_DADOS_LORAWAN_UPLINK, tamanho_item_fila);
    xQueue_display = xQueueCreate(1, sizeof( TTela_display ));

    if ( (xQueue_dados_lorawan == NULL) || (xQueue_lorawan_downlink == NULL) || (xQueue_display == NULL) )
    {
        Serial.println("[ERRO] Nao e possivel criar uma ou mais filas. ESP32 reiniciara em 1s...");
        delay(1000);
        ESP.restart();
    }

    /* Inicia o Task WDT */
    esp_task_wdt_init(TEMPO_WATCHDOG_SEGUNDOS, true); 

    /* Criação das tarefas */
    xTaskCreate(
    task_serial_rasppi               
    , "serial"                     
    ,  STACK_SIZE_SERIAL_RASPPI      
    ,  NULL                       
    ,  PRIO_SERIAL_RASPPI     
    ,  NULL);                    

    /* Criação das tarefas */
    xTaskCreatePinnedToCore(
    task_oled               
    , "task_oled"                     
    ,  STACK_SIZE_OLED      
    ,  NULL                       
    ,  PRIO_OLED     
    ,  NULL
    ,  1);

    xTaskCreate(
    task_envio_lorawan               
    , "lorawan"                     
    ,  STACK_SIZE_LORAWAN      
    ,  NULL                       
    ,  PRIO_LORAWAN     
    ,  NULL);        
}

void loop() 
{
    /* Tudo é gerenciado pelas tarefas. Nada é feito aqui. */
}

/*
 * Implementação das tarefas
 */
/* Tarefa responsável por atualizar display OLED */
void task_oled( void *pvParameters )
{
    TTela_display tela_display;

    /* Habilita o monitoramento do Task WDT nesta tarefa */
    esp_task_wdt_add(NULL); 
            
    while(1)
    {
        /* Escreve as linhas */
        if (xQueueReceive(xQueue_display, (void *)&tela_display, TICKS_ESPERA_LEITURA_FILAS) == pdTRUE) 
        {
            display.clearDisplay();
            display.setTextColor(WHITE);
            display.setCursor(0,OLED_LINHA_1);
            display.println(tela_display.linha1);
            display.setCursor(0,OLED_LINHA_2);
            display.print(tela_display.linha2);
            display.setCursor(0,OLED_LINHA_3);
            display.print(tela_display.linha3);
            display.setCursor(0,OLED_LINHA_4);
            display.print(tela_display.linha4);
            display.setCursor(0,OLED_LINHA_5);
            display.print(tela_display.linha5);
            display.setCursor(0,OLED_LINHA_6);
            display.print(tela_display.linha6);            
            display.display();
        }

        esp_task_wdt_reset();
        vTaskDelay( TEMPO_REFRESH_DISPLAY / portTICK_PERIOD_MS ); 
    }
}

/* Tarefa responsável por comunicar via serial com Raspberry Pi */ 
void task_serial_rasppi( void *pvParameters )
{
    uint8_t byte_lido;
    THeader_dados header_dados_para_rasppi;
    unsigned char array_dados_lorawan[TAMANHO_MAXIMO_DADOS+1];
    unsigned char array_dados_adc[sizeof(THeader_dados) + 15 + 1]; //1: checksum
                                                                   //15: 3 dados de 4 bytes (leituras ADC) e separadores
    unsigned char array_dados_uplink_lorawan[sizeof(THeader_dados) + 7 + 1]; //7: tamanho da string ENVIADO
                                                                             //1: checksum
    int valor_dac;
    unsigned long leitura_adc_raw;
    float tensao_adc_0;
    float tensao_adc_1;
    float tensao_adc_2;
    char dados_adc[15];
    char cks_calculado;    
    unsigned char retorno_uplink_lorawan[7] = {'E', 'N', 'V', 'I', 'A', 'D', 'O'};
    char array_dados_uplink[TAMANHO_MAXIMO_DADOS + 1] = {0};
    unsigned char buffer_display_dac[50] = {0};
    TTela_display tela_display;
    BaseType_t resultado_envio_fila_display;
    
    /* Habilita o monitoramento do Task WDT nesta tarefa */
    esp_task_wdt_add(NULL);

    memset((unsigned char *)&tela_display, 0x00, sizeof(TTela_display));

    while(1)
    {
        while (Serial1.available() )
        {
            byte_lido = Serial1.read();

            if (maquina_estados_recepcao(byte_lido) == true)
            {
                escreve_serial_debug("Chegou pacote completo!");
                
                /* Mensagem completa recebida. Faz tratamento da mesma. */
                switch(header_dados.tipo_de_dados)
                {
                    case TIPO_DADO_LORAWAN_RECEBIDO_OU_ESCRITO:
                        escreve_serial_debug("Tipo de dado: TIPO_DADO_LORAWAN_RECEBIDO_OU_ESCRITO");
                        /* Deve enviar por LoRaWAN os dados recebidos. */
                        array_dados_lorawan[0] = header_dados.tamanho;
                        memcpy(array_dados_lorawan+1, array_dados, sizeof(array_dados));
                        xQueueSend( xQueue_dados_lorawan, (void *) &array_dados_lorawan, TICKS_ESPERA_ESCRITA_FILAS);
                        escreve_serial_debug("Dados LoRaWAN colocados na fila");

                        /* Envio para Raspberry Pi */
                        header_dados_para_rasppi.byte_sincronia_1 = BYTE_SINCRONIA_1;
                        header_dados_para_rasppi.byte_sincronia_2 = BYTE_SINCRONIA_2;
                        header_dados_para_rasppi.byte_sincronia_3 = BYTE_SINCRONIA_3;
                        header_dados_para_rasppi.byte_sincronia_4 = BYTE_SINCRONIA_4;
                        header_dados_para_rasppi.byte_sincronia_5 = BYTE_SINCRONIA_5;
                        header_dados_para_rasppi.byte_sincronia_6 = BYTE_SINCRONIA_6;
                        header_dados_para_rasppi.tipo_de_dados = TIPO_DADO_LORAWAN_RECEBIDO_OU_ESCRITO;
                        header_dados_para_rasppi.tamanho = 0x07;
                        
                        cks_calculado = calcula_checksum(retorno_uplink_lorawan, sizeof(retorno_uplink_lorawan));
 
                        memcpy(array_dados_uplink_lorawan, (unsigned char *)&header_dados_para_rasppi, sizeof(THeader_dados) );
                        memcpy(array_dados_uplink_lorawan + sizeof(THeader_dados), retorno_uplink_lorawan, sizeof(retorno_uplink_lorawan) );
                        memcpy(array_dados_uplink_lorawan + sizeof(THeader_dados) + sizeof(retorno_uplink_lorawan), &cks_calculado, 1 );                        

                        Serial1.write(array_dados_uplink_lorawan, sizeof(array_dados_uplink_lorawan));
                        
                        break;

                    case TIPO_DADO_OLED_LINHA_1:
                        sprintf(tela_display.linha1, "%s", array_dados);
                        do 
                        {
                            resultado_envio_fila_display = xQueueSend(xQueue_display, (void *)&tela_display, TICKS_ESPERA_ESCRITA_FILAS);
                        }while (resultado_envio_fila_display != pdTRUE);
                        break;    

                    case TIPO_DADO_OLED_LINHA_2:
                        sprintf(tela_display.linha2, "%s", array_dados);
                        do 
                        {
                            resultado_envio_fila_display = xQueueSend(xQueue_display, (void *)&tela_display, TICKS_ESPERA_ESCRITA_FILAS);
                        }while (resultado_envio_fila_display != pdTRUE);
                        break;    
                        
                    case TIPO_DADO_OLED_LINHA_3:
                        sprintf(tela_display.linha3, "%s", array_dados);
                        do 
                        {
                            resultado_envio_fila_display = xQueueSend(xQueue_display, (void *)&tela_display, TICKS_ESPERA_ESCRITA_FILAS);
                        }while (resultado_envio_fila_display != pdTRUE);
                        break;    

                    case TIPO_DADO_OLED_LINHA_4:
                        sprintf(tela_display.linha4, "%s", array_dados);
                        do 
                        {
                            resultado_envio_fila_display = xQueueSend(xQueue_display, (void *)&tela_display, TICKS_ESPERA_ESCRITA_FILAS);
                        }while (resultado_envio_fila_display != pdTRUE);
                        break;    

                    case TIPO_DADO_OLED_LINHA_5:
                        sprintf(tela_display.linha5, "%s", array_dados);
                        do 
                        {
                            resultado_envio_fila_display = xQueueSend(xQueue_display, (void *)&tela_display, TICKS_ESPERA_ESCRITA_FILAS);
                        }while (resultado_envio_fila_display != pdTRUE);
                        break;    

                    case TIPO_DADO_OLED_LINHA_6:
                        sprintf(tela_display.linha6, "%s", array_dados);
                        do 
                        {
                            resultado_envio_fila_display = xQueueSend(xQueue_display, (void *)&tela_display, TICKS_ESPERA_ESCRITA_FILAS);
                        }while (resultado_envio_fila_display != pdTRUE);
                        break;    

                    case TIPO_DADO_ESCRITA_DAC:
                        escreve_serial_debug("Tipo de dado: TIPO_DADO_ESCRITA_DAC");
                        
                        /* Escreve no DAC o valor do primeiro byte dos dados recebidos */
                        valor_dac = 100*(array_dados[0] - 0x30) + 
                                    10*(array_dados[1] - 0x30) + 
                                    (array_dados[2] - 0x30);
                        
                        if (valor_dac > 255)
                            valor_dac = 0;
                                    
                        dacWrite(CANAL_DAC0, valor_dac);
                        escreve_serial_debug("TIPO_DADO_ESCRITA_DAC: DAC escrito com sucesso");
                        break;

                    case TIPO_DADO_LEITURA_ADC:
                        escreve_serial_debug("Tipo de dado: TIPO_DADO_LEITURA_ADC");
                        
                        /* Le o ADC (ADC 1, canais 0, 1 e 2) e manda para Raspberry Pi */
                        /* Leitura do ADC*/
                        leitura_adc_raw = adc1_get_raw(ADC1_CHANNEL_0);
                        tensao_adc_0 = esp_adc_cal_raw_to_voltage(leitura_adc_raw, &adc_cal)/1000.0;
                        
                        leitura_adc_raw = adc1_get_raw(ADC1_CHANNEL_1);
                        tensao_adc_1 = esp_adc_cal_raw_to_voltage(leitura_adc_raw, &adc_cal)/1000.0;
                        
                        leitura_adc_raw = adc1_get_raw(ADC1_CHANNEL_2);
                        tensao_adc_2 = esp_adc_cal_raw_to_voltage(leitura_adc_raw, &adc_cal)/1000.0;

                        /* Envio para Raspberry Pi */
                        header_dados_para_rasppi.byte_sincronia_1 = BYTE_SINCRONIA_1;
                        header_dados_para_rasppi.byte_sincronia_2 = BYTE_SINCRONIA_2;
                        header_dados_para_rasppi.byte_sincronia_3 = BYTE_SINCRONIA_3;
                        header_dados_para_rasppi.byte_sincronia_4 = BYTE_SINCRONIA_4;
                        header_dados_para_rasppi.byte_sincronia_5 = BYTE_SINCRONIA_5;
                        header_dados_para_rasppi.byte_sincronia_6 = BYTE_SINCRONIA_6;
                        header_dados_para_rasppi.tipo_de_dados = TIPO_DADO_LEITURA_ADC;
                        header_dados_para_rasppi.tamanho = (unsigned char)sizeof(dados_adc);

                        memset(dados_adc, 0x00, sizeof(dados_adc));
                        sprintf(dados_adc, "%.2f;%.2f;%.2f", tensao_adc_0, tensao_adc_1, tensao_adc_2);
                        cks_calculado = calcula_checksum((unsigned char *)dados_adc, sizeof(dados_adc));
 
                        memcpy(array_dados_adc, (unsigned char *)&header_dados_para_rasppi, sizeof(THeader_dados) );
                        memcpy(array_dados_adc + sizeof(THeader_dados), dados_adc, sizeof(dados_adc) );
                        memcpy(array_dados_adc + sizeof(THeader_dados) + sizeof(dados_adc), &cks_calculado, 1 );                        

                        Serial1.write(array_dados_adc, sizeof(array_dados_adc));

                        escreve_serial_debug("ADC lido com sucesso");                             
                        break;

                    default:
                        /* Se o tipo de dado for inválido, nada é feito */
                        break;
                }                
            }
            esp_task_wdt_reset();
        }

        if( xQueueReceive(xQueue_lorawan_downlink, &( array_dados_uplink ), TICKS_ESPERA_LEITURA_FILAS) == pdPASS )
        {
            header_dados_para_rasppi.byte_sincronia_1 = BYTE_SINCRONIA_1;
            header_dados_para_rasppi.byte_sincronia_2 = BYTE_SINCRONIA_2;
            header_dados_para_rasppi.byte_sincronia_3 = BYTE_SINCRONIA_3;
            header_dados_para_rasppi.byte_sincronia_4 = BYTE_SINCRONIA_4;
            header_dados_para_rasppi.byte_sincronia_5 = BYTE_SINCRONIA_5;
            header_dados_para_rasppi.byte_sincronia_6 = BYTE_SINCRONIA_6;
            header_dados_para_rasppi.tipo_de_dados = TIPO_DADO_LORAWAN_RECEBIDO_OU_ESCRITO;
            header_dados_para_rasppi.tamanho = (unsigned char)array_dados_uplink[0];
                        
            cks_calculado = calcula_checksum((unsigned char *)array_dados_uplink + 1, LMIC.dataLen);

            Serial1.write((unsigned char *)&header_dados_para_rasppi, sizeof(THeader_dados));
            Serial1.write(array_dados_uplink + 1, array_dados_uplink[0]);
            Serial1.write((unsigned char *)&cks_calculado, 1);

            escreve_serial_debug("Mensagem de downlink enviada para Raspberry Pi");  
        }
         
        /* Alimenta watchdog e aguarda 1 tick para próximo ciclo */
        esp_task_wdt_reset();
        vTaskDelay( 1 / portTICK_PERIOD_MS );  
    }
    
}

/* Tarefa responsável por enviar pacotes lorawan */
void task_envio_lorawan( void *pvParameters )
{
    unsigned char array_dados_lorawan[TAMANHO_MAXIMO_DADOS+1];
    int b;
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];

    /* Inicializa comunicação SPI com rádio LoRa */
    SPI.begin(RADIO_SCLK_PORT, RADIO_MISO_PORT, RADIO_MOSI_PORT);

    /* Inicializa stack LoRaWAN */
    os_init();
    LMIC_reset();

    /* Inicializa chaves usadas na comunicação ABP */
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
    LMIC_setSession (0x13, DEVADDR, nwkskey, appskey);

    /* Faz inicializações de rádio pertinentes a região do 
       gateway LoRaWAN (ATC / Everynet Brasil) */
    for (b=0; b<8; ++b) 
        LMIC_disableSubBand(b);

    LMIC_enableChannel(0); // 915.2 MHz
    LMIC_enableChannel(1); // 915.4 MHz
    LMIC_enableChannel(2); // 915.6 MHz
    LMIC_enableChannel(3); // 915.8 MHz
    LMIC_enableChannel(4); // 916.0 MHz
    LMIC_enableChannel(5); // 916.2 MHz
    LMIC_enableChannel(6); // 916.4 MHz
    LMIC_enableChannel(7); // 916.6 MHz

    LMIC_setAdrMode(0);
    LMIC_setLinkCheckMode(0);

    /* Data rate para janela de recepção RX2 */
    LMIC.dn2Dr = DR_SF12CR;

    /* Configura data rate de transmissão e ganho do rádio
       LoRa (dBm) na transmissão */
    LMIC_setDrTxpow(DR_SF12, GANHO_LORA_DBM);
    
    /* Habilita o monitoramento do Task WDT nesta tarefa */
    esp_task_wdt_add(NULL);

    while(1)
    {
        os_runloop_once();
        
        /* Se há ao menos um item na fila, faz o envio LoRaWAN */
        if( xQueueReceive(xQueue_dados_lorawan, &( array_dados_lorawan ), TICKS_ESPERA_LEITURA_FILAS) == pdPASS )
        {
            do_send(&sendjob, array_dados_lorawan);
        }
              
        /* Alimenta watchdog e aguarda 1 tick para próximo ciclo */
        esp_task_wdt_reset();
        vTaskDelay( 1 / portTICK_PERIOD_MS );  
    }
}
