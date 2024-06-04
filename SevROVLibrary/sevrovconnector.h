#ifndef SEVROVCONNECTOR_H
#define SEVROVCONNECTOR_H

#include <QUdpSocket>

#include "sevrovcontroldata.h"
#include "sevrovtelemetrydata.h"

const int CONTROL_PACKET_SIZE = 93;
const int TELEMETRY_PACKET_SIZE = 28;
const int CONNECT_PACKET_SIZE = 2;

class SevROVConnector : public QObject
{
    Q_OBJECT
public:
    SevROVConnector();

    enum Mode {
        UNDEFINED               = 0,
        CONTROL_READ            = 1,
        CONTROL_WRITE           = 2,
        CONTROL_READWRITE       = 3,
        TELEMETRY_READ          = 16,
        TELEMETRY_WRITE         = 32,
        TELEMETRY_READWWRITE    = 48
    };
    void setMode(std::uint8_t mode);
    std::uint8_t getMode();

    SevROVControlData control;
    SevROVTelemetryData telemetry;

    void setIP(QString ip);
    void setPort(int port);
    void writeTelemetryDatagram();    
    void writeTelemetryDatagram(QHostAddress _host, int _port);
    void writeControlDatagram();
    void writeControlDatagram(QHostAddress _host, int _port);
    void writeConnectDatagram();
    void writeConnectDatagram(QHostAddress _host, int _port);

    void openConnection();
    void openConnection(int port);
    void openConnection(QString ip, int port);
    void closeConnection();
    bool getIsConnected();

    void connectToHost(QString ip, int port);
    void disconnectFromHost();

private:
    QUdpSocket udpSocket;

    QString ip;
    QHostAddress host;
    int port;
    bool isConnected = false;

    std::uint8_t mode;

private slots:
    void processDatagram();
    void onConnected();
    void onDisconnected();

signals:
    void OnProcessDatagram();
    void OnProcessTelemetryDatagram();
    void OnProcessControlDatagram();
    void OnProcessConnectDatagram(QString ip, int port);
    void OnConnected();
    void OnDisconnected();
};

#endif // SEVROVCONNECTOR_H
