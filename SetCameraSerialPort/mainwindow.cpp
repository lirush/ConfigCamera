#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QThread *threadNew = new QThread;
    NewPort->moveToThread(threadNew);
    NewPort->thisPort.moveToThread(threadNew);
    connect(NewPort,&port::StatePort,this,&MainWindow::PrintToConsol);
    connect(threadNew, SIGNAL(started()), NewPort, SLOT(StartPort()));
    connect(NewPort, SIGNAL(finished_Port()), threadNew, SLOT(quit()));
    connect(threadNew, SIGNAL(finished()), NewPort, SLOT(deleteLater()));
    connect(NewPort, SIGNAL(finished_Port()), threadNew, SLOT(deleteLater()));
    connect(NewPort, &port::OutData, this, &MainWindow::PrintToConsol);
    connect(this,&MainWindow::SendDataToSerialPort,NewPort,&port::WriteToPort);

    SerialPortDevsSelect = new QSerialPort(this);
    GetDevsSerialPort();

    threadNew->start();
}

void MainWindow::GetDevsSerialPort()
{
    qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();
    ui->ComPorts->clear();
    SerialComPortList.clear();
    foreach(const QSerialPortInfo &SerialPortInfo, QSerialPortInfo::availablePorts())
    {
        QString dbgStr = "Vendor ID: ";
        dbgStr+= " Name: " + SerialPortInfo.portName();
        dbgStr+= " Description: "+SerialPortInfo.description();
        qDebug()<<dbgStr;
        SerialComPortList.push_back(SerialPortInfo);
        ui->ComPorts->addItem(SerialPortInfo.portName());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete SerialPortDevsSelect;
}

void MainWindow::on_Connect_clicked()
{
    if(NewPort->GetStateConnect()==false)
    {
        if(!ui->ComPorts->currentText().isEmpty())
        {
            NewPort->Settings.name = ui->ComPorts->currentText();
            NewPort->ConnectPort();
            FillFields();
            ui->Connect->setText("Disconnect");
            ui->TerminalConnect->setText(NewPort->GetSettingsPort());
            ui->ControlPanel->setEnabled(true);
            PackDataToSend(WITHOUT_PARAMETER,"read config",0);
            qDebug() << "read config send";
        }
    }
    else
    {
        NewPort->DisconnectPort();
        ui->Connect->setText("Connect");
        ui->TerminalConnect->clear();
        ui->ControlPanel->setEnabled(false);
    }
}


void MainWindow::on_SendData_clicked()
{
    if(NewPort->GetStateConnect())
    {
        QByteArray data;
        data = ui->EnterText->text().toLocal8Bit() + '\r';
        emit SendDataToSerialPort(data);
        PrintToHexConsol(data.toHex());
    }
}


void MainWindow::on_Refresh_clicked()
{
    GetDevsSerialPort();
}

void MainWindow::PrintToConsol(QString data)
{
    ui->consol->insertPlainText(data);
    ui->consol->moveCursor(QTextCursor::End);

    QByteArray hexdata;
    hexdata = data.toLocal8Bit().toHex();
    PrintToHexConsol(hexdata);

    QString temp[10];
    int i=0;
    for(auto var:data)
    {
        if(var =='\r')
        {
            temp[i] = temp[i].trimmed();
            i++;
        }
        else temp[i] += var;
    }

    if(temp[0]=="read info")
    {
        ui->InfoCam->setText(temp[1] +" " + temp[2]);
        qDebug() << "read info receive";
    }

    if(temp[0]=="read config")
    {
        bool ok;
        temp[1]=temp[1].toLocal8Bit().toHex();
        ui->bright->setText(QString::number(temp[1].mid(BRIGHTNESS*2,2).toInt(&ok,16)));
        ui->palette->setText(QString::number(temp[1].mid(PALETTE*2,2).toInt(&ok,16)));
        ui->contrast->setText(QString::number(temp[1].mid(CONTRAST*2,2).toInt(&ok,16)));
        uint16_t contrastbias = temp[1].mid(CONTRASTBIASL*2,2).toInt(&ok,16) | (temp[1].mid(CONTRASTBIASL*2,2).toInt(&ok,16)>>8);
        ui->contrast_bias->setText(QString::number(contrastbias));
        ui->gamma->setText(QString::number(temp[1].mid(GAMMA*2,2).toInt(&ok,16)));
        ui->extra_contrast->setText(QString::number(temp[1].mid(EXTRACONTRAST*2,2).toInt(&ok,16)));
        ui->destripe->setText(QString::number(temp[1].mid(DESTRIPING*2,2).toInt(&ok,16)));
        ui->sharpen->setText(QString::number(temp[1].mid(SHARPERING*2,2).toInt(&ok,16)));

        (this->*Set_P_RB[temp[1].mid(BCMODE*2,2).toInt(&ok,16)])();

        qDebug() << temp[1].mid(BCMODE*2,2).toInt(&ok,16);
    }
}

void MainWindow::PrintToHexConsol(QString data)
{
    ui->hexconsol->clear();
    ui->hexconsol->insertPlainText(data+ '\r');
    ui->hexconsol->moveCursor(QTextCursor::End);
}

void MainWindow::on_zoomplus_clicked()
{
    if(NewPort->GetStateConnect())
    {
        if(Cam->SettingsCam.zoom<4)
            Cam->SettingsCam.zoom++;
        PackDataToSend(WITH_PARAMETER,"zoom ",Cam->SettingsCam.zoom);
        ui->zoom->setText(QString::number(Cam->SettingsCam.zoom));
    }
}


void MainWindow::on_zoomminus_clicked()
{
    if(NewPort->GetStateConnect())
    {
        if(Cam->SettingsCam.zoom>0)
            Cam->SettingsCam.zoom--;

        PackDataToSend(WITH_PARAMETER,"zoom ",Cam->SettingsCam.zoom);

        ui->zoom->setText(QString::number(Cam->SettingsCam.zoom));
    }
}

void MainWindow::FillFields()
{
    if(NewPort->GetStateConnect())
    {
        PackDataToSend(WITHOUT_PARAMETER,"read info",0);
        ui->zoom->setText(QString::number(Cam->SettingsCam.zoom));
    }
}

void MainWindow::on_setbright_clicked()
{
    Cam->SettingsCam.bright = ui->bright->text().toUShort();

    ControlLimitSets(255,0,ui->bright);

    PackDataToSend(WITH_PARAMETER,"bright ",Cam->SettingsCam.bright);
}

void MainWindow::ControlLimitSets(int limup, int limdown, QLineEdit *ptr)
{
    if(ptr->text().toInt() <limdown )
        ptr->setText(QString::number(limdown));
    if(ptr->text().toInt() >limup )
        ptr->setText(QString::number(limup));
}


void MainWindow::on_setpalette_clicked()
{
    Cam->SettingsCam.palette = ui->palette->text().toUShort();

    ControlLimitSets(255,0,ui->palette);

    PackDataToSend(WITH_PARAMETER,"palette ",Cam->SettingsCam.palette);
}


void MainWindow::on_setcontrast_clicked()
{
    Cam->SettingsCam.contrast = ui->contrast->text().toUShort();

    ControlLimitSets(255,0,ui->contrast);

    PackDataToSend(WITH_PARAMETER,"contrast ",Cam->SettingsCam.contrast);
}


void MainWindow::on_readconfig_clicked()
{   
    PackDataToSend(WITHOUT_PARAMETER,"read config",0);
}


void MainWindow::on_readstatus_clicked()
{   
    PackDataToSend(WITHOUT_PARAMETER,"read status",0);
}


void MainWindow::on_restart_clicked()
{   
    PackDataToSend(WITHOUT_PARAMETER,"restart",0);
}


void MainWindow::on_setcontrast_bias_clicked()
{
    Cam->SettingsCam.contrastbias = ui->contrast_bias->text().toUShort();

    ControlLimitSets(255,0,ui->contrast_bias);

    PackDataToSend(WITH_PARAMETER,"contrast bias ",Cam->SettingsCam.contrastbias);
}


void MainWindow::on_setextracontrast_clicked()
{
    Cam->SettingsCam.extracontrast = ui->extra_contrast->text().toUShort();

    ControlLimitSets(255,0,ui->extra_contrast);

    PackDataToSend(WITH_PARAMETER,"extra contrast ",Cam->SettingsCam.extracontrast);
}


void MainWindow::on_setgamma_clicked()
{
    Cam->SettingsCam.gamma = ui->gamma->text().toUShort();

    ControlLimitSets(255,0,ui->gamma);

    PackDataToSend(WITH_PARAMETER,"gamma ",Cam->SettingsCam.gamma);
}


void MainWindow::on_setsharpen_clicked()
{
    Cam->SettingsCam.sharpen = ui->sharpen->text().toUShort();

    ControlLimitSets(255,0,ui->sharpen);

    PackDataToSend(WITH_PARAMETER,"sharpen ",Cam->SettingsCam.sharpen);
}

void MainWindow::on_setdestripe_clicked()
{
    Cam->SettingsCam.destripe = ui->destripe->text().toUShort();

    ControlLimitSets(255,0,ui->destripe);

    PackDataToSend(WITH_PARAMETER,"destripe ",Cam->SettingsCam.destripe);
}

void MainWindow::on_radioButton_clicked()
{   
    PackDataToSend(WITHOUT_PARAMETER,"bc mode 0",0);
}

void MainWindow::on_radioButton_2_clicked()
{   
    PackDataToSend(WITHOUT_PARAMETER,"bc mode 1",0);
}


void MainWindow::on_radioButton_3_clicked()
{   
    PackDataToSend(WITHOUT_PARAMETER,"bc mode 2",0);
}

void MainWindow::on_radioButton_4_clicked()
{   
    PackDataToSend(WITHOUT_PARAMETER,"bc mode 3",0);
}

void MainWindow::PackDataToSend(int packtype, QByteArray txt, uint32_t q)
{
    QByteArray data;
    switch(packtype)
    {
        case WITH_PARAMETER:
            data = txt + QByteArray::number(q) + "\r";
        break;
        case WITHOUT_PARAMETER:
            data = txt + "\r";
        break;
        default:
            break;
    }
    emit SendDataToSerialPort(data);
}

void MainWindow::on_focusnear_clicked()
{
    PackDataToSend(WITHOUT_PARAMETER,"cam drvn",0);
}

void MainWindow::on_focusfar_clicked()
{
    PackDataToSend(WITHOUT_PARAMETER,"cam drvf",0);
}

void MainWindow::on_focustop_clicked()
{
    PackDataToSend(WITHOUT_PARAMETER,"cam drvs",0);
}

void MainWindow::RB_choose0()
{
    ui->radioButton->click();
}

void MainWindow::RB_choose1()
{
    ui->radioButton_2->click();
}
void MainWindow::RB_choose2()
{
    ui->radioButton_3->click();
}
void MainWindow::RB_choose3()
{
    ui->radioButton_4->click();
}

