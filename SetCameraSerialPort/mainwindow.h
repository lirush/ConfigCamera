#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QThread>
#include <QLineEdit>
#include "port.h"
#include "camera.h"

enum PackageType{WITH_PARAMETER, WITHOUT_PARAMETER};

#define P_RB(n) (&MainWindow::RB_choose##n)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void SendDataToSerialPort(QByteArray data);

private slots:
    void on_Connect_clicked();
    void on_SendData_clicked();
    void on_Refresh_clicked();
    void on_zoomplus_clicked();
    void on_zoomminus_clicked();  
    void on_setbright_clicked();
    void on_setpalette_clicked();
    void on_setcontrast_clicked();
    void on_readconfig_clicked();
    void on_readstatus_clicked();
    void on_restart_clicked();
    void on_setcontrast_bias_clicked();
    void on_setextracontrast_clicked();
    void on_setgamma_clicked();
    void on_setsharpen_clicked();
    void on_setdestripe_clicked();
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_3_clicked();
    void on_radioButton_4_clicked();
    void on_focusnear_clicked();
    void on_focusfar_clicked();
    void on_focustop_clicked();

private:
    void GetDevsSerialPort();
    Ui::MainWindow *ui;
    QSerialPort *SerialPortDevsSelect;
    uint32_t baudrate=0;
    std::vector<QSerialPortInfo> SerialComPortList;
    bool SerialDeviceIsConnected=false;
    QString DeviceDescription;
    QString SerialBuffer;
    port *NewPort = new port();
    void PrintToConsol(QString);
    void PrintToHexConsol(QString);
    Camera *Cam = new Camera();
    void FillFields();
    void PackDataToSend(int,QByteArray,uint32_t);
    void ControlLimitSets(int limup, int limdown, QLineEdit *ptr);
    void RB_choose0();
    void RB_choose1();
    void RB_choose2();
    void RB_choose3();
    void (MainWindow::*Set_P_RB[4])()={P_RB(0),P_RB(1),P_RB(2),P_RB(3)};

};
#endif // MAINWINDOW_H
