/*
 * Para instalar o qtmqtt, use os comandos abaixo:
 *
 * sudo apt install qtdeclarative5-private-dev
 * git clone -b 5.12 https://code.qt.io/qt/qtmqtt.git
 * cd qmqtt
 * mkdir build
 * cd build
 * qmake -r ..
 */

#include "principal.h"
#include "ui_principal.h"
#include <QMqttClient>
#include <QMqttPublishProperties>
#include <QDesktopServices>
#include <QUrl>

QMqttClient *m_client;

Principal::Principal(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Principal)
{
    ui->setupUi(this);

    /* Cria client MQTT e se conecta ao broker */
    m_client = new QMqttClient(this);
    m_client->setHostname(ui->txtBroker->text() );
    m_client->setPort(ui->txtPorta->text().toInt());

    connect(m_client, &QMqttClient::connected, this, &Principal::brokerConnected);
    connect(m_client, &QMqttClient::disconnected, this, &Principal::brokerDisconnected);

    /* Quando uma mensagem for recebida, faz o tratamento */
    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {
        /* Leitura de ADCs */
        if (QString::compare(topic.name(), "mqtt_adc_para_rasppi", Qt::CaseInsensitive) == 0)
        {
            QList<QByteArray> leituras_adc = message.split(';');
            ui->lblADC1->setText(leituras_adc[0]+"V");
            ui->lblADC2->setText(leituras_adc[1]+"V");
            ui->lblADC3->setText(leituras_adc[2]+"V");
        }
    });
}



Principal::~Principal()
{
    delete ui;
}

void Principal::on_btn_ler_adcs_clicked()
{
    /* Foi pressionado botão de leitura dos ADCs */
    QString solicita_adcs = "0";
    QString topico_adcs = "mqtt_adc_para_esp32";
    m_client->publish(topico_adcs, solicita_adcs.toUtf8());
}

void Principal::on_btnConectaMQTT_clicked()
{
    if (m_client->state() == QMqttClient::Disconnected)
    {
        m_client->setHostname(ui->txtBroker->text() );
        m_client->setPort(ui->txtPorta->text().toInt());
        m_client->connectToHost();
    }
    else
    {
        m_client->disconnectFromHost();
    }
}

void Principal::brokerDisconnected()
{
    ui->lblStatus_MQTT->setText("Desconectado");
}

void Principal::brokerConnected()
{
    ui->lblStatus_MQTT->setText("Conectado");

    /* Faz subscribe nos topicos */
    QString topico_leitura_adcs = "mqtt_adc_para_rasppi";
    QString topico_downlink_lorawan = "mqtt_lorawan_downlink_esp32";
    m_client->subscribe(topico_leitura_adcs);
    m_client->subscribe(topico_downlink_lorawan);
}


void Principal::on_dialValor_DAC_valueChanged(int value)
{
    QString valor_porcentagem_dac = QString::number( (int)(((float)value/255.0)*100.0) );
    float tensao_dac = ((float)value/255.0)*3.3;
    QString valor_tensao_dac = QString::number(tensao_dac);

    /* Atualiza label com valor do DAC e tensão correspondente */
    ui->lblValor_dial->setText(valor_porcentagem_dac+"%");
    ui->lblValor_DAC_Volt->setText(valor_tensao_dac+"V");
}

void Principal::on_dialValor_DAC_sliderReleased()
{
    QString valor_ativa_dac = QString::number(ui->dialValor_DAC->value());

    /* Envia por MQTT valor a ser colocado no DAC */
    QString topico_dac = "mqtt_dac_para_esp32";
    m_client->publish(topico_dac, valor_ativa_dac.toUtf8());
}

void Principal::on_btnEscreve_OLED_clicked()
{
    QString txt_linha_1 = ui->txtOLED_Linha1->text();
    QString txt_linha_2 = ui->txtOLED_Linha2->text();
    QString txt_linha_3 = ui->txtOLED_Linha3->text();
    QString txt_linha_4 = ui->txtOLED_Linha4->text();
    QString txt_linha_5 = ui->txtOLED_Linha5->text();
    QString txt_linha_6 = ui->txtOLED_Linha6->text();

    /* Envia por MQTT valor a ser colocado na linha 1 do OLED */
    QString topico_oled_linha_1 = "mqtt_oled_linha1_esp32";
    m_client->publish(topico_oled_linha_1, txt_linha_1.toUtf8());

    /* Envia por MQTT valor a ser colocado na linha 2 do OLED */
    QString topico_oled_linha_2 = "mqtt_oled_linha2_esp32";
    m_client->publish(topico_oled_linha_2, txt_linha_2.toUtf8());

    /* Envia por MQTT valor a ser colocado na linha 3 do OLED */
    QString topico_oled_linha_3 = "mqtt_oled_linha3_esp32";
    m_client->publish(topico_oled_linha_3, txt_linha_3.toUtf8());

    /* Envia por MQTT valor a ser colocado na linha 4 do OLED */
    QString topico_oled_linha_4 = "mqtt_oled_linha4_esp32";
    m_client->publish(topico_oled_linha_4, txt_linha_4.toUtf8());

    /* Envia por MQTT valor a ser colocado na linha 5 do OLED */
    QString topico_oled_linha_5 = "mqtt_oled_linha5_esp32";
    m_client->publish(topico_oled_linha_5, txt_linha_5.toUtf8());

    /* Envia por MQTT valor a ser colocado na linha 6 do OLED */
    QString topico_oled_linha_6 = "mqtt_oled_linha6_esp32";
    m_client->publish(topico_oled_linha_6, txt_linha_6.toUtf8());
}

void Principal::on_btnEnvia_msg_Lorawan_clicked()
{
    QString topico_uplink_lorawan = "mqtt_lorawan_uplink_esp32";
    QString msg_para_enviar = ui->txtMsg_Uplink_Lorawan->text();
    m_client->publish(topico_uplink_lorawan, msg_para_enviar.toUtf8());
}

void Principal::on_btnVisitar_site_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.pedrobertoleti.com.br"));
}
