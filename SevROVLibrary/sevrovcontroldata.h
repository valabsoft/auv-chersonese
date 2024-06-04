#ifndef SEVROVCONTROLDATA_H
#define SEVROVCONTROLDATA_H


#include "sevrovdata.h"

#include <cstdint>
#include <QByteArray>
#include <QDataStream>
#include <QDateTime>

const int LIGHT_ONOFF_PAUSE = 500;

class SevROVControlData : public SevROVData
{
public:
    SevROVControlData();

    void Initialize() override;
    void Initialize(float horizontalvectorx,
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
                    float ddepthki,
                    float depthkd,
                    int8_t updatepid);

    ////////////////////////////////////////////////////////////////////////////
    // Сеттеры
    ////////////////////////////////////////////////////////////////////////////
    void setHorizontalVectorX(float value);
    void setHorizontalVectorY(float value);    
    void setVericalThrust(float value);
    void setPowerTarget(float value);
    void setAngularVelocityZ(float value);
    void setManipulatorState(float value);
    void setManipulatorRotate(float value);
    void setCameraRotate(float value);
    void setResetInitialization(int8_t value);
    void setLightsState(int8_t value);
    void setStabilizationState(int8_t value);
    void setRollInc(float value);
    void setPitchInc(float value);
    void setResetPosition(int8_t value);
    void setRollKp(float value);
    void setRollKi(float value);
    void setRollKd(float value);
    void setPitchKp(float value);
    void setPitchKi(float value);
    void setPitchKd(float value);
    void setYawKp(float value);
    void setYawKi(float value);
    void setYawKd(float value);
    void setDepthKp(float value);
    void setDepthKi(float value);
    void setDepthKd(float value);
    void setUpdatePID(int8_t value);
    ////////////////////////////////////////////////////////////////////////////
    // Геттеры
    ////////////////////////////////////////////////////////////////////////////
    float getHorizontalVectorX();
    float getHorizontalVectorY();
    float getPowerTarget();
    float getVericalThrust();
    float getAngularVelocityZ();
    float getManipulatorState();
    float getManipulatorRotate();
    float getCameraRotate();
    int8_t getResetInitialization();
    int8_t getLightsState();
    int8_t getStabilizationState();
    float getRollInc();
    float getPitchInc();
    int8_t getResetPosition();
    float getRollKp();
    float getRollKi();
    float getRollKd();
    float getPitchKp();
    float getPitchKi();
    float getPitchKd();
    float getYawKp();
    float getYawKi();
    float getYawKd();
    float getDepthKp();
    float getDepthKi();
    float getDepthKd();
    int8_t getUpdatePID();

    QByteArray toByteArray() override;
    void printDebugInfo() override;

    QDateTime LightsStatePrevious;

private:
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
};

#endif // SEVROVCONTROLDATA_H
