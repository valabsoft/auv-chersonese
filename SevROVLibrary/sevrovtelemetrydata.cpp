#include "sevrovtelemetrydata.h"
#include "qdebug.h"

SevROVTelemetryData::SevROVTelemetryData()
{

}

void SevROVTelemetryData::Initialize()
{
    Roll = 0.0;
    Pitch = 0.0;
    Yaw = 0.0;
    Heading = 0.0;
    Depth = 0.0;
    RollSetPoint = 0.0;
    PitchSetPoint = 0.0;
}

void SevROVTelemetryData::Initialize(float roll,
                                     float pitch,
                                     float yaw,
                                     float heading,
                                     float depth,
                                     float rollsetpoint,
                                     float pitchsetpoint)
{
    Roll = roll;
    Pitch = pitch;
    Yaw = yaw;
    Heading = heading;
    Depth = depth;
    RollSetPoint = rollsetpoint;
    PitchSetPoint = pitchsetpoint;
}

void SevROVTelemetryData::setRoll(float value)
{
    Roll = value;
}
void SevROVTelemetryData::setPitch(float value)
{
    Pitch = value;
}
void SevROVTelemetryData::setYaw(float value)
{
    Yaw = value;
}
void SevROVTelemetryData::setHeading(float value)
{
    Heading = value;
}
void SevROVTelemetryData::setDepth(float value)
{
    Depth = value;
}
void SevROVTelemetryData::setRollSetPoint(float value)
{
    RollSetPoint = value;
}
void SevROVTelemetryData::setPitchSetPoint(float value)
{
    PitchSetPoint = value;
}

float SevROVTelemetryData::getRoll()
{
    return Roll;
}
float SevROVTelemetryData::getPitch()
{
    return Pitch;
}
float SevROVTelemetryData::getYaw()
{
    return Yaw;
}
float SevROVTelemetryData::getHeading()
{
    return Heading;
}
float SevROVTelemetryData::getDepth()
{
    return Depth;
}
float SevROVTelemetryData::getRollSetPoint()
{
    return RollSetPoint;
}
float SevROVTelemetryData::getPitchSetPoint()
{
    return PitchSetPoint;
}

QByteArray SevROVTelemetryData::toByteArray()
{
    QByteArray result;
    QDataStream stream(&result, QIODeviceBase::WriteOnly);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_6_3);

    stream << Roll;
    stream << Pitch;
    stream << Yaw;
    stream << Heading;
    stream << Depth;
    stream << RollSetPoint;
    stream << PitchSetPoint;

    return result;
}
void SevROVTelemetryData::printDebugInfo()
{
    std::string datetime = QDateTime::currentDateTime()
                               .toString("dd/MM/yyyy hh:mm:ss").toStdString();
    quint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "=============================================================";
    qDebug() << "TELEMETRY" << this->toByteArray().size() << "[bytes]"
             << datetime.c_str() << "[" << timestamp << "]";
    qDebug() << "=============================================================";
    qDebug() << "Roll:\t\t\t" << toFloatString(Roll).c_str();
    qDebug() << "Pitch:\t\t\t" << toFloatString(Pitch).c_str();
    qDebug() << "Yaw:\t\t\t" << toFloatString(Yaw).c_str();
    qDebug() << "Heading:\t\t\t" << toFloatString(Heading).c_str();
    qDebug() << "Depth:\t\t\t" << toFloatString(Depth).c_str();
    qDebug() << "RollSetPoint:\t\t\t" << toFloatString(RollSetPoint).c_str();
    qDebug() << "PitchSetPoint:\t\t\t" << toFloatString(PitchSetPoint).c_str();
    qDebug() << "";
}
