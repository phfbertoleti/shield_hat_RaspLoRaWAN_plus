#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Principal; }
QT_END_NAMESPACE

class Principal : public QMainWindow
{
    Q_OBJECT

public:
    Principal(QWidget *parent = nullptr);
    ~Principal();

private slots:
    void on_btn_ler_adcs_clicked();
    void on_btnConectaMQTT_clicked();
    void brokerDisconnected();
    void brokerConnected();
    void on_dialValor_DAC_valueChanged(int value);
    void on_dialValor_DAC_sliderReleased();

    void on_btnEscreve_OLED_clicked();

    void on_btnEnvia_msg_Lorawan_clicked();

    void on_btnVisitar_site_clicked();

private:
    Ui::Principal *ui;
};
#endif // PRINCIPAL_H
