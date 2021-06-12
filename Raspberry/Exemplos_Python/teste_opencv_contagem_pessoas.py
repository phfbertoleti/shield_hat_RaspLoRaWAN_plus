# Exemplo: projeto que identifica pessoas (faces e corpos) e envia
#          contabilização via LoRaWAN
#
# Importante: para compilar e instalar o OpenCV, siga o tutorial
# descrito em: https://linuxize.com/post/how-to-install-opencv-on-raspberry-pi/
# Autor: Pedro Bertoleti

import paho.mqtt.client as mqtt
import cv2
import sys
import time

# Definicoes:
Broker = "127.0.0.1"
Porta_Broker = 1883
Keep_Alive_Broker = 60
Timeout_Conexao = 5

# Definicoes - LoRaWAN
contador_transmissoes = 0

# Definicoes - contadores de pessoas
contador_pessoas = 0
max_contador_pessoas_periodo = 0

# Callback - conexao ao broker realizada
def on_connect(client, userdata, flags, rc):
    print("[STATUS] Conectado ao Broker. Resultado de conexao: "+str(rc))
    return

# Callback - mensagem recebida do broker
def on_message(client, userdata, msg):
    msg_recebida = str(msg.payload)
    print("Mensagem MQTT recebida: "+msg_recebida)
    return

# Funcao: obtem o maximo valor dentre dois informados
# Parametros: valores
# Retorno: valor maximo
def maximo(a,b):
    if (a>b):
        return a

    return b

#####################
# Programa principal
#####################

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
    try:
        # Carrega o cascade que detecta faces humanas
        cascade_face = cv2.CascadeClassifier('/usr/local/share/opencv4/haarcascades/haarcascade_frontalface_default.xml')

        # Carrega o cascade que detecta corpos humanos
        cascade_corpos = cv2.CascadeClassifier('/usr/local/share/opencv4/haarcascades/haarcascade_fullbody.xml')

        # Captura imagem e converte para tons de cinza
        camera = cv2.VideoCapture(0)
        ret, frame = camera.read()
        cinza = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        # Detecta faces e corpos
        faces = cascade_face.detectMultiScale(cinza, 1.1, 4)
        corpos = cascade_corpos.detectMultiScale(cinza, 1.1, 4)

        # Conta as faces e corpos detectadas
        contador_faces = 0
        for (x, y, w, h) in faces:
            # cv2.rectangle(frame, (x, y), (x+w, y+h), (255, 0, 0), 2)
            contador_faces = contador_faces + 1

        contador_corpos = 0
        for (x, y, w, h) in corpos:
            # cv2.rectangle(frame, (x, y), (x+w, y+h), (255, 0, 0), 2)
            contador_corpos = contador_corpos + 1

        # Contabiliza pessoas e maximo de pessoas
        contador_pessoas = maximo(contador_faces, contador_corpos)

        if (contador_pessoas > max_contador_pessoas_periodo):
            max_contador_pessoas_periodo = contador_pessoas

        print("Faces detectadas: "+str(contador_faces))
        camera.release()

        # Escreve no display do Heltec ESP32 LoRa WiFi o numero de faces
        # detectadas via camera e OpenCV
        client.publish("mqtt_oled_linha1_esp32", "Faces: "+str(contador_faces))
        client.publish("mqtt_oled_linha2_esp32", "Max: "+str(max_contador_pessoas_periodo))

        # Se for o momento (30s apos ultimo envio), envia contagem por LoRaWAN
        if ( (time.time() - timestamp_ref) >= 30):
            timestamp_ref = time.time()
            str_envio_lorawan = str(contador_faces)+";"+str(max_contador_pessoas_periodo)
            client.publish("mqtt_lorawan_uplink_esp32", str_envio_lorawan)
            contador_transmissoes = contador_transmissoes + 1
            print("Contagem de transmissoes LoRaWAN: "+str(contador_transmissoes))
            max_contador_pessoas_periodo = 0

        # Reconecta ao broker MQTT caso a conexao caia
        status_conexao = client.loop(Timeout_Conexao)
        if (status_conexao > 0):
            client.connectclient.connect(Broker, Porta_Broker, Keep_Alive_Broker)

    except KeyboardInterrupt:
        print("\nCtrl+C pressionado, encerrando aplicacao e saindo...")
        sys.exit(0)
