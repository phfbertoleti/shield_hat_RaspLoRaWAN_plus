# Exemplo: envio de mensagens LoRaWAN (uplink) via
#          Heltec ESP32 WiFi LoRaWAN
# Autor: Pedro Bertoleti

import paho.mqtt.client as mqtt
import time
import sys

#definicoes: 
Broker = "127.0.0.1"
Porta_Broker = 1883
Keep_Alive_Broker = 60
Timeout_Conexao = 5

# defnicoes - LoRaWAN
contador_transmissoes = 0

#Callback - conexao ao broker realizada
def on_connect(client, userdata, flags, rc):
    print("[STATUS] Conectado ao Broker. Resultado de conexao: "+str(rc))
    client.subscribe("mqtt_adc_para_rasppi")
    return

#Callback - mensagem recebida do broker
def on_message(client, userdata, msg):
    global contador_transmissoes

    msg_recebida = str(msg.payload)

    # envia medicoes recebidas por lorawan
    contador_transmissoes = contador_transmissoes + 1
    client.publish("mqtt_lorawan_uplink_esp32", msg_recebida.replace("b'", "").replace("'", ""))
    print("Transmissoes LoRaWAN: "+str(contador_transmissoes))
 
    #escreve medicoes no display
    leituras_adc = msg_recebida.split(";")
    client.publish("mqtt_oled_linha1_esp32", "ADC0: "+leituras_adc[0].replace("b'", "").replace("'", "")+"V")
    client.publish("mqtt_oled_linha2_esp32", "ADC1: "+leituras_adc[1].replace("b'", "").replace("'", "")+"V")
    client.publish("mqtt_oled_linha3_esp32", "ADC2: "+leituras_adc[2].replace("b'", "").replace("'", "")+"V")
    client.publish("mqtt_oled_linha4_esp32", " ")
    client.publish("mqtt_oled_linha5_esp32", " ")
    client.publish("mqtt_oled_linha6_esp32", " ")
    return

#programa principal:
print("[STATUS] Inicializando MQTT...")

#inicializa MQTT:
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(Broker, Porta_Broker, Keep_Alive_Broker)

#limpa display
client.publish("mqtt_oled_linha1_esp32", " ")
client.publish("mqtt_oled_linha2_esp32", " ")
client.publish("mqtt_oled_linha3_esp32", " ")
client.publish("mqtt_oled_linha4_esp32", " ")
client.publish("mqtt_oled_linha5_esp32", " ")
client.publish("mqtt_oled_linha6_esp32", " ")

timestamp_ref = time.time()
while True:
    status_conexao = client.loop(Timeout_Conexao)
    if (status_conexao > 0):
        client.connectclient.connect(Broker, Porta_Broker, Keep_Alive_Broker)

    #obtem leituras do ADC e transmite via LoRaWAN a cada 30s
    if ( (time.time() - timestamp_ref) >= 30):
        timestamp_ref = time.time()
        #solicita leitura do ADC
        client.publish("mqtt_adc_para_esp32", "0")        
