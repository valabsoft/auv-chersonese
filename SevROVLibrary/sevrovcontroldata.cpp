#include "sevrovcontroldata.h"
#include "qdebug.h"

SevROVControlData::SevROVControlData()
{
    LightsStatePrevious = QDateTime::currentDateTime();
}

void SevROVControlData::Initialize()
{
    HorizontalVectorX = 0.0;
    HorizontalVectorY = 0.0;
    VericalThrust = 0.0;
    PowerTarget = 0.0;
    AngularVelocityZ = 0.0;
    ManipulatorState = 0.0;
    ManipulatorRotate = 0.0;
    CameraRotate = 0.0;
    ResetInitialization = 0;
    LightsState = 0;
    StabilizationState = 0;
    RollInc = 0.0;
    PitchInc = 0.0;
    ResetPosition = 0;
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
    UpdatePID = 0.0;
}

void SevROVControlData::Initialize(float horizontalvectorx,
                                   float horizontalvectory,
                                   float vericalthrust,
                                   float powertarget,
                                   float angularvelocityz,
                                   float manipulatorstate,
                                   float manipulatorrotate,
                                   float camerarotate,
                                   int8_t resetinitialization,
                                   int8_t lightsstate,
                                   int8_t stabilizationstate,
                                   float rollinc,
                                   float pitchinc,
                                   int8_t resetposition,
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
                                   float depthkd,
                                   int8_t updatepid)
{
    HorizontalVectorX = horizontalvectorx;
    HorizontalVectorY = horizontalvectory;
    VericalThrust = vericalthrust;
    PowerTarget = powertarget;
    AngularVelocityZ = angularvelocityz;
    ManipulatorState = manipulatorstate;
    ManipulatorRotate = manipulatorrotate;
    CameraRotate = camerarotate;
    ResetInitialization = resetinitialization;
    LightsState = lightsstate;
    StabilizationState = stabilizationstate;
    RollInc = rollinc;
    PitchInc = pitchinc;
    ResetPosition = resetposition;
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
    UpdatePID = updatepid;
}

void SevROVControlData::setHorizontalVectorX(float value)
{
    HorizontalVectorX = value;
}
void SevROVControlData::setHorizontalVectorY(float value)
{
    HorizontalVectorY = value;
}
void SevROVControlData::setVericalThrust(float value)
{
    VericalThrust = value;
}
void SevROVControlData::setPowerTarget(float value)
{
    PowerTarget = value;
}
void SevROVControlData::setAngularVelocityZ(float value)
{
    AngularVelocityZ = value;
}
void SevROVControlData::setManipulatorState(float value)
{
    ManipulatorState = value;
}
void SevROVControlData::setManipulatorRotate(float value)
{
    ManipulatorRotate = value;
}
void SevROVControlData::setCameraRotate(float value)
{
    CameraRotate = value;
}
void SevROVControlData::setResetInitialization(int8_t value)
{
    ResetInitialization = value;
}
void SevROVControlData::setLightsState(int8_t value)
{
    QDateTime LightsStateCurrent = QDateTime::currentDateTime();

    //qDebug() << LightsStatePrevious;
    //qDebug() << LightsStateCurrent;
    //qDebug() << LightsStatePrevious.msecsTo(LightsStateCurrent);

    // Смена режима не раньше чем раз в X секунд
    if (LightsStatePrevious.msecsTo(LightsStateCurrent) >= LIGHT_ONOFF_PAUSE)
    {
        LightsState = value;
        LightsStatePrevious = QDateTime::currentDateTime();
    }
}
void SevROVControlData::setStabilizationState(int8_t value)
{
    StabilizationState = value;
}
void SevROVControlData::setRollInc(float value)
{
    RollInc = value;
}
void SevROVControlData::setPitchInc(float value)
{
    PitchInc = value;
}
void SevROVControlData::setResetPosition(int8_t value)
{
    ResetPosition = value;
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
void SevROVControlData::setUpdatePID(int8_t value)
{
    UpdatePID = value;
}

float SevROVControlData::getHorizontalVectorX()
{
    return HorizontalVectorX;
}
float SevROVControlData::getHorizontalVectorY()
{
    return HorizontalVectorY;
}
float SevROVControlData::getVericalThrust()
{
    return VericalThrust;
}
float SevROVControlData::getPowerTarget()
{
    return PowerTarget;
}
float SevROVControlData::getAngularVelocityZ()
{
    return AngularVelocityZ;
}
float SevROVControlData::getManipulatorState()
{
    return ManipulatorState;
}
float SevROVControlData::getManipulatorRotate()
{
    return ManipulatorRotate;
}
float SevROVControlData::getCameraRotate()
{
    return CameraRotate;
}
int8_t SevROVControlData::getResetInitialization()
{
    return ResetInitialization;
}
int8_t SevROVControlData::getLightsState()
{
    return LightsState;
}
int8_t SevROVControlData::getStabilizationState()
{
    return StabilizationState;
}
float SevROVControlData::getRollInc()
{
    return RollInc;
}
float SevROVControlData::getPitchInc()
{
    return PitchInc;
}
int8_t SevROVControlData::getResetPosition()
{
    return ResetPosition;
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
int8_t SevROVControlData::getUpdatePID()
{
    return UpdatePID;
}


QByteArray SevROVControlData::toByteArray()
{
    QByteArray result;
    QDataStream stream(&result, QIODeviceBase::WriteOnly);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_6_3);

    stream << HorizontalVectorX;
    stream << HorizontalVectorY;
    stream << VericalThrust;
    stream << PowerTarget;
    stream << AngularVelocityZ;
    stream << ManipulatorState;
    stream << ManipulatorRotate;
    stream << CameraRotate;
    stream << ResetInitialization;
    stream << LightsState;
    stream << StabilizationState;
    stream << RollInc;
    stream << PitchInc;
    stream << ResetPosition;
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
    stream << UpdatePID;

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
    qDebug() << "HorizontalVectorX:\t" << toFloatString(HorizontalVectorX).c_str();
    qDebug() << "HorizontalVectorY:\t" << toFloatString(HorizontalVectorY).c_str();
    qDebug() << "VericalThrust:\t\t" << toFloatString(VericalThrust).c_str();
    qDebug() << "PowerTarget:\t\t" << toFloatString(PowerTarget).c_str();
    qDebug() << "AngularVelocityZ:\t\t" << toFloatString(AngularVelocityZ).c_str();
    qDebug() << "ManipulatorState:\t\t" << toFloatString(ManipulatorState).c_str();
    qDebug() << "ManipulatorRotate:\t" << toFloatString(ManipulatorRotate).c_str();
    qDebug() << "CameraRotate:\t\t" << toFloatString(CameraRotate).c_str();
    qDebug() << "ResetInitialization:\t" << toIntString(ResetInitialization).c_str();
    qDebug() << "LightsState:\t\t" << toIntString(LightsState).c_str();
    qDebug() << "StabilizationState:\t" << toIntString(StabilizationState).c_str();
    qDebug() << "RollInc:\t\t\t" << toIntString(RollInc).c_str();
    qDebug() << "PitchInc:\t\t" << toIntString(PitchInc).c_str();
    qDebug() << "ResetPosition:\t\t" << toIntString(ResetPosition).c_str();
    //qDebug() << "RollKp:\t\t" << toIntString(RollKp).c_str();
    //qDebug() << "RollKi:\t\t" << toIntString(RollKi).c_str();
    //qDebug() << "RollKd:\t\t" << toIntString(RollKd).c_str();
    //qDebug() << "PitchKp:\t\t" << toIntString(PitchKp).c_str();
    //qDebug() << "PitchKi:\t\t" << toIntString(PitchKi).c_str();
    //qDebug() << "PitchKd:\t\t" << toIntString(PitchKd).c_str();
    //qDebug() << "YawKp:\t\t" << toIntString(YawKp).c_str();
    //qDebug() << "YawKi:\t\t" << toIntString(YawKi).c_str();
    //qDebug() << "YawKd:\t\t" << toIntString(YawKd).c_str();
    //qDebug() << "DepthKp:\t\t" << toIntString(DepthKp).c_str();
    //qDebug() << "DepthKi:\t\t" << toIntString(DepthKi).c_str();
    //qDebug() << "DepthKd:\t\t" << toIntString(DepthKd).c_str();
    qDebug() << "UpdatePID:\t\t" << toIntString(UpdatePID).c_str();
    qDebug() << "";
}
