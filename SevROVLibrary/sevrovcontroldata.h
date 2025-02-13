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
    void Initialize(uint64_t flags,
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
                    float depthkd);

    ////////////////////////////////////////////////////////////////////////////
    // Сеттеры
    ////////////////////////////////////////////////////////////////////////////

    void setFlags(uint64_t value);
    void setMoveForward(float value);
    void setMoveSideways(float value);
    void setMoveVertical(float value);
    void setRotateYaw(float value);
    void setIncrementRoll(float value);
    void setIncrementPitch(float value);
    void setPowerSetPoint(float value);
    void setRotateCamera(float value);
    void setGrabManipulator(float value);
    void setRotateManipulator(float value);
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

    ////////////////////////////////////////////////////////////////////////////
    // Геттеры
    ////////////////////////////////////////////////////////////////////////////
    uint64_t getFlags();
    float getMoveForward();
    float getMoveSideways();
    float getMoveVertical();
    float getRotateYaw();
    float getIncrementRoll();
    float getIncrementPitch();
    float getPowerSetPoint();
    float getRotateCamera();
    float getGrabManipulator();
    float getRotateManipulator();
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

    QByteArray toByteArray() override;
    void printDebugInfo() override;

    QDateTime LightsStatePrevious;

private:
    uint64_t Flags; // Флаги управления
    float MoveForward; // Движение вперед [-1..1]
    float MoveSideways; // Движение в сторону [-1..1]
    float MoveVertical; // Движение по вертикали (погружение / всплытие) [-1..1]
    float RotateYaw; // Вращение по курсу [-1..1]
    float IncrementRoll; // Инкремент крена [-1;0;1]
    float IncrementPitch; // Инкремент дифферента [-1;0;1]
    float PowerSetPoint; // Уставка мощности [0..1]
    float RotateCamera; // Вращение камеры [-1;0;1]
    float GrabManipulator; // Схват манипулятора [-1..1]
    float RotateManipulator; // Вращение манипулятора [-1..1]
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
};

#endif // SEVROVCONTROLDATA_H
