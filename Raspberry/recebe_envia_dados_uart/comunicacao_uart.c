/* Módulo: comunicação UART com o ESP32 (Heltec ESP32 WiFi LoRa)
 * Autor: Pedro Bertoleti 
 */

#include <fcntl.h> 
#include <termios.h>
#include <unistd.h>
#include <errno.h> 
#include <stdio.h>
#include <stdint.h>
#include "comunicacao_uart.h"
#include "debug_defs.h"

/* Função: abre comunicação da serial com ESP32
 * Parâmetros: nenhum
 * Retorno: file descriptor da serial
 */
int abre_serial(void)
{
    int fd = open(UART_TTGO_PATH, O_RDWR | O_NOCTTY);
    int status_serial_attr;
    int status_gravacao_config_serial;
    struct termios options;
  
    if (fd <= 0)
    {
        #ifdef HABILITA_DEBUG_VIA_PRINTF 
        printf("\r\n [ERRO] Nao foi possivel se comunicar com a serial.\r\n");
        #endif

        return -1;
    }
 
    /* Limpa buffers de escrita e leitura da UART */
    int status_flush = tcflush(fd, TCIOFLUSH);
    if (status_flush)
    {
        #ifdef HABILITA_DEBUG_VIA_PRINTF
        printf("\r\n [ALERTA] Nao foi possivel limpar buffers da serial.\r\n");
        #endif
    }
 
    /* Le configuração atual da serial */
    status_serial_attr = tcgetattr(fd, &options);
    if (status_serial_attr)
    {
        #ifdef HABILITA_DEBUG_VIA_PRINTF
        printf("\r\n [ERRO] Nao foi possivel obter configuracoes atuais da serial.\r\n");
        #endif

        close(fd);
        return -1;
    }
 
    /* Desliga sinais de controle e finaliza leitura quando:
       - Existir ao menos um byte a ser lido OU
       - Se em 100ms nada foi lido 
    */
    options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
    options.c_oflag &= ~(ONLCR | OCRNL);
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    options.c_cc[VTIME] = 1;
    options.c_cc[VMIN] = 0;
 
    /* Configura baud rate */
    cfsetospeed(&options, BAUDRATE_UART_TTGO);   
    cfsetispeed(&options, cfgetospeed(&options));
 
    /* Grava nova configuração da serial */
    status_gravacao_config_serial = tcsetattr(fd, TCSANOW, &options);
    if (status_gravacao_config_serial)
    {
        #ifdef HABILITA_DEBUG_VIA_PRINTF 
        printf("\r\n [ERRO] Nao foi possivel gravar configuracoes da serial.\r\n");
        #endif

        close(fd);
        return -1;
    }
 
    return fd;
}

/* Função: escreve bytes na serial que está o ESP32
 * Parâmetros: - file descriptor da serial
 *             - ponteiro para o buffer a ser escrito
 *             - tamanho do buffer a ser escrito
 * Retorno: 0: escrita ok
 *         -1: falha ao escrever bytes
 */
int escreve_bytes(int fd, uint8_t * pt_buffer_escrita, size_t tamanho_buffer_escrita)
{
    int escrita_na_serial = 0;
    ssize_t status_escrita_serial = write(fd, pt_buffer_escrita, tamanho_buffer_escrita);
  
    if (status_escrita_serial != (ssize_t)tamanho_buffer_escrita)
    {
        #ifdef HABILITA_DEBUG_VIA_PRINTF
        printf("\r\n [ERRO] Nao foi possivel escrever bytes na serial.\r\n");
        #endif

        escrita_na_serial = -1;
    }
  
    return escrita_na_serial;
}

/* Função: le bytes da serial que está o ESP32
 * Parâmetros: - file descriptor da serial
 *             - ponteiro para o buffer de leitura
 *             - tamanho do buffer de leitura
 * Retorno: número de bytes lidos
 */
ssize_t le_bytes(int fd, uint8_t * pt_buffer_leitura, size_t tamanho_buffer_leitura)
{
    size_t bytes_lidos = 0;
    ssize_t qtde_bytes_lidos;
    int bytes_para_ler = 0;
    
    while (bytes_lidos < tamanho_buffer_leitura)
    {
        bytes_para_ler = tamanho_buffer_leitura - bytes_lidos;
        qtde_bytes_lidos = read(fd, pt_buffer_leitura + bytes_lidos, bytes_para_ler);
        
        if (qtde_bytes_lidos < 0)
        {
            #ifdef HABILITA_DEBUG_VIA_PRINTF
            printf("\r\n [ERRO] Impossivel ler bytes da serial.\r\n");
            #endif
            
            return -1;
        }
    
        if (qtde_bytes_lidos == 0)
            break;
    
        bytes_lidos = bytes_lidos + qtde_bytes_lidos;
    }

    return bytes_lidos;
}