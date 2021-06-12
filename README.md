# Repositório - Shield/hat RaspLoRaWAN+

Este é o repositório oficial do shield/hat RaspLoRaWAN+. Este sheild/hat fornece à Raspberry Pi, com o uso de um Heltec ESP32 WiFi LoRa, os seguintes recursos:

* Envio (uplink) e recepção (downlink) de mensagens LoRaWAN (já configurado para operar com a rede da ATC no Brasil)
* Leitura de até 3 canais ADC (tensão máxima: 3V3)
* Controle de uma saída analógica (via DAC, com tensão máxima de 3V3)
* Interação com dispositivos (sensores, por exemplo) I²C, com nível de tensão de 3V3
* Escrita, linha a linha, no display OLED do Heltec ESP32 WiFi LoRa

![Foto do shield/hat RaspLoraWAN+](https://github.com/phfbertoleti/shield_hat_RaspLoRaWAN_plus/Imagens/shield_rasplorawan+_1.jpg)

## Funcionamento e estrutura básica dos códigos-fonte

Este shield/hat funciona com a Raspberry Pi enviando e recebendo mensagens ao Heltec ESP32 WiFi LoRa via UART (115200/8/N/1). Para tal, do lado da Raspberry Pi, é disponível um serviço chamado recebe_envia_dados_uart, totalmente escrito em C para máxima eficiência e otimização. Já do lado do Heltec ESP32 WiFi LoRa, é utilizado um software embarcado escrito na Arduino IDE, fazendo uso de FreeRTOS.

Para interagir com o serviço, é utilizado o protocolo MQTT como IPC (Inter-Processes Communication), com um broker MQTT rodando internamente na Raspberry Pi. Dessa forma, é possível interagir com todas as funcionalidades fazendo publish e subscribe em tópicos MQTT específicos.

A estrutura básica dos códigos-fonte é:

* *Pasta ESP32:* software embarcado a ser gravado no Heltec ESP32 WiFi LoRa. Antes de compilar e gravar, é preciso somente colocar as chaves LoRaWAN no arquivo LORAWAN_defs.h
* *Pasta Raspberry/recebe_envia_dados_uart:* serviço de comunicação UART entre Raspberry Pi e Heltec ESP32 WiFi LoRa, assim como IPC com MQTT
* *Pasta Raspberry/unit_files:* unit files para habilitar o serviço no systemd
* *Pasta Raspberry/Exemplos_Python:* exemplos (em Python 3.X) que exploram todos os recursos da placa
* *Pasta Hardware:* projeto completo (esquemático, layout e gerber files) de hardware do shield/hat

## Compatibilidade

Este shield/hat e seu respectivo software é compatível com todas Raspberries Pi com header de expansão de 40 pinos, como por exemplo:

* Raspberry Pi 3B
* Raspberry Pi 3B+
* Raspberry Pi Zero
* Raspberry Pi Zero W
* Raspberry Pi 4
* Raspberry Pi A+
* etc.

## Instalação dos softwares necessários na Raspberry Pi

Para instalar todos os softwares e dependências necessários na Raspberry Pi, siga o procedimento abaixo:

1. Se sua Raspberry Pi não possuir acesso à Internet, faça-a ter. 
Isso é necessário para a instalação das dependências.
2. Clone este repositório no home (/home/pi)
3. Na pasta do repositório, execute o script instala_tudo.sh como super-usuário:

```
sudo instala_tudo.sh
```

4. Aguarde a instalação automática. No fim do processo, o serviço que comunica com o shield/hat via UART e demais processos (IPC) via MQTT estará rodando. Para verificar a execução deste serviço, utilize o comando abaixo:

```
sudo systemctl status recebe_envia_esp32.service
```