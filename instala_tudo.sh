#! /usr/bin/bash

# Shield RaspLoRaWAN+: script de instalação das ferramentas e serviços 
#
# Este script prepara a Raspberry Pi para utilizar o sheild RaspLoRaWAN+,
# instalando todas as ferramentas e serviços necessários para seu funcionamento.
#
# Autor: Pedro Bertoleti
# Data: Junho/2021

if (( $EUID != 0 )); then
    echo "ERRO: esse script precisa ser executado como root. Utilize o comando 'sudo ./instala_tudo.sh'"
    exit
fi

echo "##########################"
echo "#  Etapa 1: dependencias #"
echo "##########################"
echo " "

echo " "
echo "- Dependencias: apt-get"

sudo apt-get update

# Verifica as dependencias (apt-get)
array_dep_cmd=("python3" \
               "pip3" \
               "mosquitto" \
               "git" )

array_dep_aptget=("python3" \
                  "python3-pip" \
                  "mosquitto" \
                  "git" )       

dep_idx=0
for dep_atual in "${array_dep_cmd[@]}"
do
    if command -v $dep_atual >/dev/null 2>&1
    then
        echo "* Dependencia $dep_atual ja instalada."
    else
        echo "* Dependencia $dep_atual nao encontrada. Fazendo instalacao..."
        sudo apt-get -y install ${array_dep_aptget[$dep_idx]}
    fi
    dep_idx=$(( $dep_idx+1 ))
done

# Instala a biblioteca Paho-MQTT (lib python)
echo " "
echo "- Dependencias: lib python paho-mqtt"
sudo pip3 install paho-mqtt

# Instala a biblioteca Paho-MQTT (lib C)
echo " "
echo "- Dependencias: lib C paho-mqtt"

LIB_PAHO_MQTT_C_1='/usr/local/lib/libpaho-mqtt3c.so'
LIB_PAHO_MQTT_C_2='/usr/lib/libpaho-mqtt3c.so'
LIB_PAHO_MQTT_C_3='/lib/libpaho-mqtt3c.so'

if [[ -s $LIB_PAHO_MQTT_C_1 ]] || [[ -s $LIB_PAHO_MQTT_C_2 ]] || [[ -s $LIB_PAHO_MQTT_C_3 ]]
then
    echo "lib C paho-mqtt ja esta instalada."
else
    echo "Instalando lib C paho-mqtt..."
    sudo apt-get -y install libssl-dev doxygen
    git clone https://github.com/eclipse/paho.mqtt.c.git
    cd paho.mqtt.c
    make -j4
    make html
    sudo make install
fi

echo "#######################################################"
echo "#  Etapa 2: compilacao do servico (comunicacao ESP32) #"
echo "#######################################################"
echo " "
echo " "
cd Raspberry/recebe_envia_dados_uart
make
mkdir /home/pi/recebe_envia_dados_uart
cp recebe_envia_dados_uart /home/pi/recebe_envia_dados_uart
cd ../..

echo "###############################################################"
echo "#  Etapa 3: habilita o servico (comunicacao ESP32) no systemd #"
echo "###############################################################"
echo " "
echo " "
sudo cp Raspberry/unit_files/recebe_envia_esp32.service /etc/systemd/system/
sudo systemctl start recebe_envia_esp32.service
sudo systemctl enable recebe_envia_esp32.service

echo " "
echo " "
echo "########################################################################"
echo "#  Fim da instalacao. Software do shiels Rasp LoRaWAN pronto para uso! #"
echo "########################################################################"
echo " "
echo " "