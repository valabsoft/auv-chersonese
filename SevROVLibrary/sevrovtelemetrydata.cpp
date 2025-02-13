#include "sevrovtelemetrydata.h"
#include "qdebug.h"

SevROVTelemetryData::SevROVTelemetryData()
{
    ;
}

void SevROVTelemetryData::Initialize()
{
    Flags = 0;
    Roll = 0.0;
    Pitch = 0.0;
    Yaw = 0.0;
    Depth = 0.0;
    BatteryVoltage = 0.0;
    BatteryChargeLevel = 0.0;
    CurrentConsumption = 0.0;
    RollSetPoint = 0.0;
    PitchSetPoint = 0.0;
}

void SevROVTelemetryData::Initialize(uint64_t flags,
                                     float roll,
                                     float pitch,
                                     float yaw,
                                     float depth,
                                     float batteryvoltage,
                                     float batterychargeLevel,
                                     float currentconsumption,
                                     float rollsetpoint,
                                     float pitchsetpoint)
{
    Flags = flags;
    Roll = roll;
    Pitch = pitch;
    Yaw = yaw;
    Depth = depth;
    BatteryVoltage = batteryvoltage;
    BatteryChargeLevel = batterychargeLevel;
    CurrentConsumption = currentconsumption;
    RollSetPoint = rollsetpoint;
    PitchSetPoint = pitchsetpoint;
}

void SevROVTelemetryData::setFlags(uint64_t value)
{
    Flags = value;
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
void SevROVTelemetryData::setDepth(float value)
{
    Depth = value;
}
void SevROVTelemetryData::setBatteryVoltage(float value)
{
    BatteryVoltage = value;
}
void SevROVTelemetryData::setBatteryChargeLevel(float value)
{
    BatteryChargeLevel = value;
}
void SevROVTelemetryData::setCurrentConsumption(float value)
{
    CurrentConsumption = value;
}
void SevROVTelemetryData::setRollSetPoint(float value)
{
    RollSetPoint = value;
}
void SevROVTelemetryData::setPitchSetPoint(float value)
{
    PitchSetPoint = value;
}

uint64_t SevROVTelemetryData::getFlags()
{
    return Flags;
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
float SevROVTelemetryData::getDepth()
{
    return Depth;
}
float SevROVTelemetryData::getBatteryVoltage()
{
    return BatteryVoltage;
}
float SevROVTelemetryData::getBatteryChargeLevel()
{
    return BatteryChargeLevel;
}
float SevROVTelemetryData::getCurrentConsumption()
{
    return CurrentConsumption;
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

    stream << Flags;
    stream << Roll;
    stream << Pitch;
    stream << Yaw;
    stream << Depth;
    stream << BatteryVoltage;
    stream << BatteryChargeLevel;
    stream << CurrentConsumption;
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
    qDebug() << "Flags:\t\t\t" << toUIntString(Flags).c_str();
    qDebug() << "Roll:\t\t\t" << toFloatString(Roll).c_str();
    qDebug() << "Pitch:\t\t\t" << toFloatString(Pitch).c_str();
    qDebug() << "Yaw:\t\t\t" << toFloatString(Yaw).c_str();    
    qDebug() << "Depth:\t\t\t" << toFloatString(Depth).c_str();

    qDebug() << "BatteryVoltage:\t\t\t" << toFloatString(BatteryVoltage).c_str();
    qDebug() << "BatteryChargeLevel:\t\t\t" << toFloatString(BatteryChargeLevel).c_str();
    qDebug() << "CurrentConsumption:\t\t\t" << toFloatString(CurrentConsumption).c_str();

    qDebug() << "RollSetPoint:\t\t\t" << toFloatString(RollSetPoint).c_str();
    qDebug() << "PitchSetPoint:\t\t\t" << toFloatString(PitchSetPoint).c_str();
    qDebug() << "";
}
