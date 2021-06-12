# Exemplo: teste do canal 1 do DAC do Heltec ESP32 WiFi LoRaWAN.
#          O valor da saida analogica eh escrito no display
#          do Heltec ESP32 WiFi LoRa.
# Autor: Pedro Bertoleti

import paho.mqtt.client as mqtt
import time
import sys

# Definicoes:
Broker = "127.0.0.1"
Porta_Broker = 1883
Keep_Alive_Broker = 60
Timeout_Conexao = 5

# Callback - conexao ao broker realizada
def on_connect(client, userdata, flags, rc):
    print("[STATUS] Conectado ao Broker. Resultado de conexao: "+str(rc))
    return

# Callback - mensagem recebida do broker
def on_message(client, userdata, msg):
	msg_recebida = str(msg.payload)
	print("[MSG RECEBIDA] Topico: "+msg.topic+" / Mensagem: "+msg_recebida)

#programa principal:
print("[STATUS] Inicializando MQTT...")

#inicializa MQTT:
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(Broker, Porta_Broker, Keep_Alive_Broker)

valor_dac = 0
while True:
   try:
        status_conexao = client.loop(Timeout_Conexao)
        if (status_conexao > 0):
            client.connectclient.connect(Broker, Porta_Broker, Keep_Alive_Broker)

        valor_dac = valor_dac + 1

        if (valor_dac == 256):
            valor_dac = 0

        print("Valor DAC: "+str(valor_dac))
        client.publish("mqtt_dac_para_esp32", str(valor_dac))

        time.sleep(0.1)
   except KeyboardInterrupt:
        print("\nCtrl+C pressionado, encerrando aplicacao e saindo...")
        sys.exit(0)
