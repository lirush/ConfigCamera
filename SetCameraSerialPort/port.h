#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QSerialPort>

struct SettingsPort{
    QString name;
    qint32 baudrate;
    QSerialPort::DataBits databits;
    QSerialPort::StopBits stopbits;
    QSerialPort::Parity parity;
    QSerialPort::FlowControl flowcontrol;
};

enum State {
    Disconnected = 0,
    Connected = 1
};

class port : public QObject
{
    Q_OBJECT
public:
    explicit port(QObject *parent = nullptr);
    ~port();
    QSerialPort thisPort;
    SettingsPort Settings;

signals:
    void FinishedPort();
    void StatePort(QString);
    void OutData(QString);

public slots:
    void ConnectPort();
    void DisconnectPort();
    bool GetStateConnect();
    QString GetSettingsPort();
    void StartPort();
    void WriteToPort(QByteArray data);
    void ReadInPort();

private:
    bool fStatePort=false;

};

#endif // PORT_H
