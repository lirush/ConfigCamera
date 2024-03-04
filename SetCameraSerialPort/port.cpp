#include "port.h"
#include <qdebug.h>

port::port(QObject *parent)
    : QObject{parent}
{
    Settings.baudrate = QSerialPort::Baud57600;
    Settings.databits = QSerialPort::Data8;
    Settings.parity = QSerialPort::NoParity;
    Settings.stopbits = QSerialPort::OneStop;
    Settings.flowcontrol = QSerialPort::NoFlowControl;
}

port::~port()
{
    fStatePort = Disconnected;
    emit FinishedPort();
}

void port::ConnectPort()
{
    thisPort.setPortName(Settings.name);
    if (thisPort.open(QIODevice::ReadWrite))
    {
        if (thisPort.setBaudRate(Settings.baudrate)
                && thisPort.setDataBits(Settings.databits)
                && thisPort.setParity(Settings.parity)
                && thisPort.setStopBits(Settings.stopbits)
                && thisPort.setFlowControl(Settings.flowcontrol))
        {
            if(thisPort.isOpen())
            {
                fStatePort = Connected;
                emit StatePort((Settings.name+ " >> open!\r").toLocal8Bit());
                qDebug() << "is connected";
            }
        }
        else
        {
            thisPort.close();
            fStatePort = Disconnected;
            emit StatePort((Settings.name+ " >> close!\r").toLocal8Bit());
            qDebug() << "is not connected because not settings";
        }
    }
    else
    {
        thisPort.close();
        emit StatePort((Settings.name+ " >> close!\r").toLocal8Bit());
        qDebug() << "is not connected";
    }
}

void port::DisconnectPort()
{
    if(thisPort.isOpen())
    {
        thisPort.close();
        fStatePort = Disconnected;
        emit StatePort((Settings.name+ " >> close!\r").toLocal8Bit());
        qDebug() << "disconnected";
    }
}

bool port::GetStateConnect()
{
    return fStatePort;
}

QString port::GetSettingsPort()
{
    QString info;
    info = "baudrate: " + QString::number(thisPort.baudRate(QSerialPort::AllDirections)) + "\r";
    info += "data: " + QString::number(thisPort.dataBits()) + "\r";
    QString txt;
    switch(thisPort.parity())
    {
    case QSerialPort::NoParity:
        txt = "none";
        break;
    case QSerialPort::EvenParity:
        txt = "even";
        break;
    default:
        break;
    }
    info += "parity: " + txt + "\r";
    info += "stopbits: " + QString::number(thisPort.stopBits());
    return info;
}

void port::StartPort()
{
    qDebug() << "Start Port";
    connect(&thisPort, SIGNAL(readyRead()),this,SLOT(ReadInPort()));
}

void port :: WriteToPort(QByteArray data)
{
    if(thisPort.isOpen())
    {
        thisPort.write(data);
        qDebug() << "data send";
    }
}
//
void port :: ReadInPort()
{
    QByteArray data;
    data.append(thisPort.readAll());
    emit OutData(data);
    qDebug() << "data receive";
}
