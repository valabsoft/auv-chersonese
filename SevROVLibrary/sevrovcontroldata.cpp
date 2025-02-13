#include "sevrovcontroldata.h"
#include "qdebug.h"

SevROVControlData::SevROVControlData()
{
    LightsStatePrevious = QDateTime::currentDateTime();
}

void SevROVControlData::Initialize()
{
    Flags = 0;
    MoveForward = 0.0;
    MoveSideways = 0.0;
    MoveVertical = 0.0;
    RotateYaw = 0.0;
    IncrementRoll = 0.0;
    IncrementPitch = 0.0;
    PowerSetPoint = 0.0;
    RotateCamera = 0.0;
    GrabManipulator = 0.0;
    RotateManipulator = 0.0;
    RollKp = 0.0;
    RollKi = 0.0;
    RollKd = 0.0;
    PitchKp = 0.0;
    PitchKi = 0.0;
    PitchKd = 0.0;
    YawKp = 0.0;
    YawKi = 0.0;
    YawKd = 0.0;
    DepthKp = 0.0;
    DepthKi = 0.0;
    DepthKd = 0.0;
}
void SevROVControlData::Initialize(
    uint64_t flags,
    float moveforward,
    float movesideways,
    float movevertical,
    float rotateyaw,
    float incrementroll,
    float incrementpitch,
    float powersetpoint,
    float rotatecamera,
    float grabmanipulator,
    float rotatemanipulator,
    float rollkp,
    float rollki,
    float rollkd,
    float pitchkp,
    float pitchki,
    float pitchkd,
    float yawkp,
    float yawki,
    float yawkd,
    float depthkp,
    float depthki,
    float depthkd)
{
    Flags = flags;
    MoveForward = moveforward;
    MoveSideways = movesideways;
    MoveVertical = movevertical;
    RotateYaw = rotateyaw;
    IncrementRoll = incrementroll;
    IncrementPitch = incrementpitch;
    PowerSetPoint = powersetpoint;
    RotateCamera = rotatecamera;
    GrabManipulator = grabmanipulator;
    RotateManipulator = rotatemanipulator;
    RollKp = rollkp;
    RollKi = rollki;
    RollKd = rollkd;
    PitchKp = pitchkp;
    PitchKi = pitchki;
    PitchKd = pitchkd;
    YawKp = yawkp;
    YawKi = yawki;
    YawKd = yawkd;
    DepthKp = depthkp;
    DepthKi = depthki;
    DepthKd = depthkd;
}

void SevROVControlData::setFlags(uint64_t value)
{
    Flags = value;
}
void SevROVControlData::setMoveForward(float value)
{
    MoveForward = value;
}
void SevROVControlData::setMoveSideways(float value)
{
    MoveSideways = value;
}
void SevROVControlData::setMoveVertical(float value)
{
    MoveVertical = value;
}
void SevROVControlData::setRotateYaw(float value)
{
    RotateYaw = value;
}
void SevROVControlData::setIncrementRoll(float value)
{
    IncrementRoll = value;
}
void SevROVControlData::setIncrementPitch(float value)
{
    IncrementPitch = value;
}
void SevROVControlData::setPowerSetPoint(float value)
{
    PowerSetPoint = value;
}
void SevROVControlData::setRotateCamera(float value)
{
    RotateCamera = value;
}
void SevROVControlData::setGrabManipulator(float value)
{
    GrabManipulator = value;
}
void SevROVControlData::setRotateManipulator(float value)
{
    RotateManipulator = value;
}
void SevROVControlData::setRollKp(float value)
{
    RollKp = value;
}
void SevROVControlData::setRollKi(float value)
{
    RollKi = value;
}
void SevROVControlData::setRollKd(float value)
{
    RollKd = value;
}
void SevROVControlData::setPitchKp(float value)
{
    PitchKp = value;
}
void SevROVControlData::setPitchKi(float value)
{
    PitchKi = value;
}
void SevROVControlData::setPitchKd(float value)
{
    PitchKd = value;
}
void SevROVControlData::setYawKp(float value)
{
    YawKp = value;
}
void SevROVControlData::setYawKi(float value)
{
    YawKi = value;
}
void SevROVControlData::setYawKd(float value)
{
    YawKd = value;
}
void SevROVControlData::setDepthKp(float value)
{
    DepthKp = value;
}
void SevROVControlData::setDepthKi(float value)
{
    DepthKi = value;
}
void SevROVControlData::setDepthKd(float value)
{
    DepthKd = value;
}

uint64_t SevROVControlData::getFlags()
{
    return Flags;
}
float SevROVControlData::getMoveForward()
{
    return MoveForward;
}
float SevROVControlData::getMoveSideways()
{
    return MoveSideways;
}
float SevROVControlData::getMoveVertical()
{
    return MoveVertical;
}
float SevROVControlData::getRotateYaw()
{
    return RotateYaw;
}
float SevROVControlData::getIncrementRoll()
{
    return IncrementRoll;
}
float SevROVControlData::getIncrementPitch()
{
    return IncrementPitch;
}
float SevROVControlData::getPowerSetPoint()
{
    return PowerSetPoint;
}
float SevROVControlData::getRotateCamera()
{
    return RotateCamera;
}
float SevROVControlData::getGrabManipulator()
{
    return GrabManipulator;
}
float SevROVControlData::getRotateManipulator()
{
    return RotateManipulator;
}
float SevROVControlData::getRollKp()
{
    return RollKp;
}
float SevROVControlData::getRollKi()
{
    return RollKi;
}
float SevROVControlData::getRollKd()
{
    return RollKd;
}
float SevROVControlData::getPitchKp()
{
    return PitchKp;
}
float SevROVControlData::getPitchKi()
{
    return PitchKi;
}
float SevROVControlData::getPitchKd()
{
    return PitchKd;
}
float SevROVControlData::getYawKp()
{
    return YawKp;
}
float SevROVControlData::getYawKi()
{
    return YawKi;
}
float SevROVControlData::getYawKd()
{
    return YawKd;
}
float SevROVControlData::getDepthKp()
{
    return DepthKp;
}
float SevROVControlData::getDepthKi()
{
    return DepthKi;
}
float SevROVControlData::getDepthKd()
{
    return DepthKd;
}

QByteArray SevROVControlData::toByteArray()
{
    QByteArray result;
    QDataStream stream(&result, QIODeviceBase::WriteOnly);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_6_3);

    stream << Flags;
    stream << MoveForward;
    stream << MoveSideways;
    stream << MoveVertical;
    stream << RotateYaw;
    stream << IncrementRoll;
    stream << IncrementPitch;
    stream << PowerSetPoint;
    stream << RotateCamera;
    stream << GrabManipulator;
    stream << RotateManipulator;
    stream << RollKp;
    stream << RollKi;
    stream << RollKd;
    stream << PitchKp;
    stream << PitchKi;
    stream << PitchKd;
    stream << YawKp;
    stream << YawKi;
    stream << YawKd;
    stream << DepthKp;
    stream << DepthKi;
    stream << DepthKd;

    return result;
}

void SevROVControlData::printDebugInfo()
{
    std::string datetime = QDateTime::currentDateTime()
                               .toString("dd/MM/yyyy hh:mm:ss").toStdString();
    quint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "=============================================================";
    qDebug() << "CONTROL" << this->toByteArray().size() << "[bytes]"
             << datetime.c_str() << "[" << timestamp << "]";
    qDebug() << "=============================================================";

    qDebug() << "Flags:\t" << toUIntString(Flags).c_str();
    qDebug() << "MoveForward:\t" << toFloatString(MoveForward).c_str();
    qDebug() << "MoveSideways:\t\t" << toFloatString(MoveSideways).c_str();
    qDebug() << "MoveVertical:\t\t" << toFloatString(MoveVertical).c_str();
    qDebug() << "RotateYaw:\t\t" << toFloatString(RotateYaw).c_str();
    qDebug() << "IncrementRoll:\t\t" << toFloatString(IncrementRoll).c_str();
    qDebug() << "IncrementPitch:\t" << toFloatString(IncrementPitch).c_str();
    qDebug() << "PowerSetPoint:\t\t" << toFloatString(PowerSetPoint).c_str();
    qDebug() << "RotateCamera:\t" << toFloatString(RotateCamera).c_str();
    qDebug() << "GrabManipulator:\t\t" << toFloatString(GrabManipulator).c_str();
    qDebug() << "RotateManipulator:\t" << toFloatString(RotateManipulator).c_str();
    qDebug() << "";
}
