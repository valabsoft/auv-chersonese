#include "sevrovconnector.h"

SevROVConnector::SevROVConnector()
{
    // Неопределенный режим функционирования
    setMode(SevROVConnector::Mode::UNDEFINED);

    // Инициализация управления
    control.Initialize();
    // Инициализация телеметрии
    telemetry.Initialize();

    connect(&udpSocket, SIGNAL(readyRead()), this, SLOT(processDatagram()));
    // TODO: На данный момент события сокета connected/disconnected
    // не вызываются. Для этого нужен connectToHost.
    // Но тогда проблема с пакетами...
    connect(&udpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(&udpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void SevROVConnector::setIP(QString ip)
{
    this->ip = ip;
    this->host = QHostAddress(ip);
}

void SevROVConnector::setPort(int port)
{
    this->port = port;
}

void SevROVConnector::writeTelemetryDatagram()
{
    // Проверяем, разрешено ли коннектору писать телеметрию
    if ((mode & SevROVConnector::Mode::TELEMETRY_WRITE)
        == SevROVConnector::Mode::TELEMETRY_WRITE)
    {
        qint64 bytes = udpSocket.writeDatagram(telemetry.toByteArray(), host, port);
        qDebug() << "[" << QDateTime::currentMSecsSinceEpoch()
                 << "] Telemetry datagram:" << host.toString().toStdString().c_str()
                 << ":" << port << "- packet size:" << bytes << "[bytes]";
    }
}

void SevROVConnector::writeTelemetryDatagram(QHostAddress _host, int _port)
{
    // Проверяем, разрешено ли коннектору писать телеметрию
    if ((mode & SevROVConnector::Mode::TELEMETRY_WRITE)
        == SevROVConnector::Mode::TELEMETRY_WRITE)
    {
        qint64 bytes = udpSocket.writeDatagram(telemetry.toByteArray(), _host, _port);
        qDebug() << "[" << QDateTime::currentMSecsSinceEpoch()
                 << "] Telemetry datagram:" << _host.toString().toStdString().c_str()
                 << ":" << _port << "- packet size:" << bytes << "[bytes]";
    }
}

void SevROVConnector::writeControlDatagram()
{
    // Проверяем, разрешено ли коннектору писать телеметрию
    if ((mode & SevROVConnector::Mode::CONTROL_WRITE)
        == SevROVConnector::Mode::CONTROL_WRITE)
    {
        qint64 bytes = udpSocket.writeDatagram(control.toByteArray(), host, port);
        qDebug() << "[" << QDateTime::currentMSecsSinceEpoch()
                 << "] Control datagram:" << host.toString().toStdString().c_str()
                 << ":" << port << "- packet size:" << bytes << "[bytes]";
    }
}

void SevROVConnector::writeControlDatagram(QHostAddress _host, int _port)
{
    // Проверяем, разрешено ли коннектору писать телеметрию
    if ((mode & SevROVConnector::Mode::CONTROL_WRITE)
        == SevROVConnector::Mode::CONTROL_WRITE)
    {
        qint64 bytes = udpSocket.writeDatagram(control.toByteArray(), _host, _port);
        qDebug() << "[" << QDateTime::currentMSecsSinceEpoch()
                 << "] Control datagram:" << _host.toString().toStdString().c_str()
                 << ":" << _port << "- packet size:" << bytes << "[bytes]";
    }
}

void SevROVConnector::writeConnectDatagram()
{
    QByteArray bytearray;
    QDataStream stream(&bytearray, QIODeviceBase::WriteOnly);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_6_3);

    stream << (std::byte)0xAA;
    stream << (std::byte)0xFF;

    qint64 bytes = udpSocket.writeDatagram(bytearray, host, port);
    qDebug() << "[" << QDateTime::currentMSecsSinceEpoch()
             << "] Connect datagram:" << host.toString().toStdString().c_str()
             << ":" << port << "- packet size:" << bytes << "[bytes]";

}
void SevROVConnector::writeConnectDatagram(QHostAddress _host, int _port)
{
    QByteArray bytearray;
    QDataStream stream(&bytearray, QIODeviceBase::WriteOnly);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_6_3);
    stream << (std::byte)0xAA;
    stream << (std::byte)0xFF;

    qint64 bytes = udpSocket.writeDatagram(bytearray, _host, _port);
    qDebug() << "[" << QDateTime::currentMSecsSinceEpoch()
             << "] Connect datagram:" << _host.toString().toStdString().c_str()
             << ":" << _port << "- packet size:" << bytes << "[bytes]";
}

void SevROVConnector::processDatagram()
{
    if (udpSocket.pendingDatagramSize() == TELEMETRY_PACKET_SIZE)
    {
        // Проверяем, разрешено ли коннектору читать телеметрию
        if ((mode & SevROVConnector::Mode::TELEMETRY_READ)
            == SevROVConnector::Mode::TELEMETRY_READ)
        {
            QByteArray datagram;
            do {
                datagram.resize(udpSocket.pendingDatagramSize());
                udpSocket.readDatagram(datagram.data(), datagram.size());
            } while (udpSocket.hasPendingDatagrams());

            QDataStream in(&datagram, QIODevice::ReadOnly);
            in.setFloatingPointPrecision(QDataStream::SinglePrecision);
            in.setByteOrder(QDataStream::LittleEndian);
            in.setVersion(QDataStream::Qt_6_3);

            float Roll;
            float Pitch;
            float Yaw;
            float Heading;
            float Depth;
            float RollSetPoint;
            float PitchSetPoint;

            in >> Roll;
            in >> Pitch;
            in >> Yaw;
            in >> Heading;
            in >> Depth;
            in >> RollSetPoint;
            in >> PitchSetPoint;

            telemetry.setRoll(Roll);
            telemetry.setPitch(Pitch);
            telemetry.setYaw(Yaw);
            telemetry.setHeading(Heading);
            telemetry.setDepth(Depth);
            telemetry.setRollSetPoint(RollSetPoint);
            telemetry.setPitchSetPoint(PitchSetPoint);

            emit OnProcessTelemetryDatagram();
        }
    }

    if (udpSocket.pendingDatagramSize() == CONTROL_PACKET_SIZE)
    {
        // Проверяем, разрешено ли коннектору читать телеметрию
        if ((mode & SevROVConnector::Mode::CONTROL_READ)
            == SevROVConnector::Mode::CONTROL_READ)
        {
            QByteArray datagram;
            QHostAddress senderAddress = QHostAddress::Null;
            quint16 senderPort = 0;

            do {
                datagram.resize(udpSocket.pendingDatagramSize());
                udpSocket.readDatagram(datagram.data(), datagram.size(),
                                       &senderAddress, &senderPort);
            } while (udpSocket.hasPendingDatagrams());

            QDataStream in(&datagram, QIODevice::ReadOnly);
            in.setFloatingPointPrecision(QDataStream::SinglePrecision);
            in.setByteOrder(QDataStream::LittleEndian);
            in.setVersion(QDataStream::Qt_6_3);

            float HorizontalVectorX;
            float HorizontalVectorY;
            float VericalThrust;
            float PowerTarget;
            float AngularVelocityZ;
            float ManipulatorState;
            float ManipulatorRotate;
            float CameraRotate;
            int8_t ResetInitialization;
            int8_t LightsState;
            int8_t StabilizationState;
            float RollInc;
            float PitchInc;
            int8_t ResetPosition;
            float RollKp;
            float RollKi;
            float RollKd;
            float PitchKp;
            float PitchKi;
            float PitchKd;
            float YawKp;
            float YawKi;
            float YawKd;
            float DepthKp;
            float DepthKi;
            float DepthKd;
            int8_t UpdatePID;

            in >> HorizontalVectorX;
            in >> HorizontalVectorY;
            in >> VericalThrust;
            in >> PowerTarget;
            in >> AngularVelocityZ;
            in >> ManipulatorState;
            in >> ManipulatorRotate;
            in >> CameraRotate;
            in >> ResetInitialization;
            in >> LightsState;
            in >> StabilizationState;
            in >> RollInc;
            in >> PitchInc;
            in >> ResetPosition;
            in >> RollKp;
            in >> RollKi;
            in >> RollKd;
            in >> PitchKp;
            in >> PitchKi;
            in >> PitchKd;
            in >> YawKp;
            in >> YawKi;
            in >> YawKd;
            in >> DepthKp;
            in >> DepthKi;
            in >> DepthKd;
            in >> UpdatePID;

            control.setHorizontalVectorX(HorizontalVectorX);
            control.setHorizontalVectorY(HorizontalVectorY);
            control.setVericalThrust(VericalThrust);
            control.setPowerTarget(PowerTarget);
            control.setAngularVelocityZ(AngularVelocityZ);
            control.setManipulatorState(ManipulatorState);
            control.setManipulatorRotate(ManipulatorRotate);
            control.setCameraRotate(CameraRotate);
            control.setResetInitialization(ResetInitialization);
            control.setLightsState(LightsState);
            control.setStabilizationState(StabilizationState);
            control.setRollInc(RollInc);
            control.setPitchInc(PitchInc);
            control.setResetPosition(ResetPosition);
            control.setRollKp(RollKp);
            control.setRollKi(RollKi);
            control.setRollKd(RollKd);
            control.setPitchKp(PitchKp);
            control.setPitchKi(PitchKi);
            control.setPitchKd(PitchKd);
            control.setYawKp(YawKp);
            control.setYawKi(YawKi);
            control.setYawKd(YawKd);
            control.setDepthKp(DepthKp);
            control.setDepthKi(DepthKi);
            control.setDepthKd(DepthKd);
            control.setUpdatePID(UpdatePID);

            emit OnProcessControlDatagram();

            // Если коннектор должен ответить на датаграмму -
            // отправляем телеметрию
            if ((mode & SevROVConnector::Mode::TELEMETRY_WRITE)
                == SevROVConnector::Mode::TELEMETRY_WRITE)
            {
                if ((senderAddress != QHostAddress::Null) && (senderPort != 0))
                    writeTelemetryDatagram(senderAddress, senderPort);
            }
        }
    }

    if (udpSocket.pendingDatagramSize() == CONNECT_PACKET_SIZE)
    {
        QByteArray datagram;
        QHostAddress senderAddress = QHostAddress::Null;
        quint16 senderPort = 0;

        do {
            datagram.resize(udpSocket.pendingDatagramSize());
            udpSocket.readDatagram(datagram.data(), datagram.size(),
                                   &senderAddress, &senderPort);
        } while (udpSocket.hasPendingDatagrams());

        QDataStream in(&datagram, QIODevice::ReadOnly);
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setVersion(QDataStream::Qt_6_3);

        float A;
        float B;

        in >> A;
        in >> B;

        QString senderIP = senderAddress.toString();

        emit OnProcessConnectDatagram(senderIP, senderPort);
    }
}

// !!! Должно использоваться только для коннектора в режиме сервера !!!
void SevROVConnector::openConnection()
{
    qDebug() << "openConnection(): " << host.toString().toStdString().c_str() << ":" << port;
    // Открываем канал для входящих сообщений
    bool result = udpSocket.bind(host, port);

    if (result)
        onConnected();
    else
        onDisconnected();
}
// !!! Должно использоваться только для коннектора в режиме сервера !!!
void SevROVConnector::openConnection(int port)
{
    this->ip = "127.0.0.1";
    this->host = QHostAddress(ip);
    this->port = port;

    qDebug() << "openConnection(): " << host.toString().toStdString().c_str() << ":" << port;
    bool result = udpSocket.bind(host, port);

    if (result)
        onConnected();
    else
        onDisconnected();
}

// !!! Должно использоваться только для коннектора в режиме сервера !!!
void SevROVConnector::openConnection(QString ip, int port)
{    
    this->ip = ip;
    this->host = QHostAddress(ip);
    this->port = port;

    qDebug() << "openConnection(): " << host.toString().toStdString().c_str() << ":" << port;
    bool result = udpSocket.bind(host, port);

    if (result)
        onConnected();
    else
        onDisconnected();
}
// !!! Должно использоваться только для коннектора в режиме сервера !!!
void SevROVConnector::closeConnection()
{
    qDebug() << "closeConnection(): " << host.toString().toStdString().c_str() << ":" << port;
    udpSocket.close();
}

void SevROVConnector::onConnected()
{
    isConnected = true;
    emit OnConnected();
}

void SevROVConnector::onDisconnected()
{
    isConnected = false;
    emit OnDisconnected();
}

bool SevROVConnector::getIsConnected()
{
    return isConnected;
}

void SevROVConnector::setMode(std::uint8_t mode)
{
    this->mode = mode;
}

std::uint8_t SevROVConnector::getMode()
{
    return mode;
}

void SevROVConnector::connectToHost(QString ip, int port)
{
    qDebug() << "connectToHost(): " << QHostAddress(ip).toString().toStdString().c_str() << ":" << port;
    udpSocket.connectToHost(QHostAddress(ip), port, QIODeviceBase::ReadWrite);
}
void SevROVConnector::disconnectFromHost()
{
    qDebug() << "disconnectFromHost(): " << QHostAddress(ip).toString().toStdString().c_str() << ":" << port;
    udpSocket.disconnectFromHost();
}
