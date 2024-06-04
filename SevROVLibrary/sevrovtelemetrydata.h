#ifndef SEVROVTELEMETRYDATA_H
#define SEVROVTELEMETRYDATA_H

#include "sevrovdata.h"

#include <QByteArray>
#include <QDataStream>
#include <QDateTime>

class SevROVTelemetryData : public SevROVData
{
public:
    SevROVTelemetryData();

    void Initialize() override;
    void Initialize(float roll,
                    float pitch,
                    float yaw,
                    float heading,
                    float depth,
                    float rollsetpoint,
                    float pitchsetpoint);

    void setRoll(float value);
    void setPitch(float value);
    void setYaw(float value);
    void setHeading(float value);
    void setDepth(float value);
    void setRollSetPoint(float value);
    void setPitchSetPoint(float value);

    float getRoll();
    float getPitch();
    float getYaw();
    float getHeading();
    float getDepth();
    float getRollSetPoint();
    float getPitchSetPoint();

    QByteArray toByteArray() override;
    void printDebugInfo() override;

private:
    float Roll;
    float Pitch;
    float Yaw;
    float Heading;
    float Depth;
    float RollSetPoint;
    float PitchSetPoint;
};

#endif // SEVROVTELEMETRYDATA_H
