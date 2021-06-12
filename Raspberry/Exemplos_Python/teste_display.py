# Exemplo: teste de escrita de mensagens no display
# OLED do Heltec ESP32 WiFi LoRaWAN
# Autor: Pedro Bertoleti

import paho.mqtt.client as mqtt
import time
import sys

#definicoes: 
Broker = "127.0.0.1"
Porta_Broker = 1883
Keep_Alive_Broker = 60
Timeout_Conexao = 5

#Callback - conexao ao broker realizada
def on_connect(client, userdata, flags, rc):
    print("[STATUS] Conectado ao Broker. Resultado de conexao: "+str(rc))
    return

#Callback - mensagem recebida do broker
def on_message(client, userdata, msg):
	MensagemRecebida = str(msg.payload)
    
	print("[MSG RECEBIDA] Topico: "+msg.topic+" / Mensagem: "+MensagemRecebida)
	

#programa principal:
print("[STATUS] Inicializando MQTT...")

#inicializa MQTT:
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(Broker, Porta_Broker, Keep_Alive_Broker)

while True:   
   try:
        status_conexao = client.loop(Timeout_Conexao)
        if (status_conexao > 0):
            client.connectclient.connect(Broker, Porta_Broker, Keep_Alive_Broker)
   
        client.publish("mqtt_oled_linha1_esp32", "Linha 1")
        client.publish("mqtt_oled_linha2_esp32", "Linha 2")
        client.publish("mqtt_oled_linha3_esp32", "Linha 3")
        client.publish("mqtt_oled_linha4_esp32", "Linha 4")
        client.publish("mqtt_oled_linha5_esp32", "Linha 5")
        client.publish("mqtt_oled_linha6_esp32", "Linha 6")

        time.sleep(1)
   except KeyboardInterrupt:
        print("\nCtrl+C pressionado, encerrando aplicacao e saindo...")
        sys.exit(0)
