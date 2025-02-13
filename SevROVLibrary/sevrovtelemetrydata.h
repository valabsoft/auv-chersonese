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
    void Initialize(
        uint64_t flags,
        float roll,
        float pitch,
        float yaw,
        float depth,
        float batteryvoltage,
        float batterychargeLevel,
        float currentconsumption,
        float rollsetpoint,
        float pitchsetpoint);

    void setFlags(uint64_t value);
    void setRoll(float value);
    void setPitch(float value);
    void setYaw(float value);
    void setDepth(float value);
    void setBatteryVoltage(float value);
    void setBatteryChargeLevel(float value);
    void setCurrentConsumption(float value);
    void setRollSetPoint(float value);
    void setPitchSetPoint(float value);

    uint64_t getFlags();
    float getRoll();
    float getPitch();
    float getYaw();
    float getDepth();
    float getBatteryVoltage();
    float getBatteryChargeLevel();
    float getCurrentConsumption();
    float getRollSetPoint();
    float getPitchSetPoint();

    QByteArray toByteArray() override;
    void printDebugInfo() override;

private:
    uint64_t Flags; // Флаги телеметрии
    float Roll; // Крен
    float Pitch; // Дифферент - Тангаж
    float Yaw; // Курс - Рысканье
    float Depth; // Глубина
    float BatteryVoltage; // Напряжение аккумулятора
    float BatteryChargeLevel; // Уровень заряда батареи
    float CurrentConsumption; // Потребляемый ток
    float RollSetPoint; // Уставка стабилизации по крену
    float PitchSetPoint; // Уставка стабилизации по дифференту
};

#endif // SEVROVTELEMETRYDATA_H
