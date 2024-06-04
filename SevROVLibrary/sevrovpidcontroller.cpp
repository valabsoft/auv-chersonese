#include "sevrovpidcontroller.h"

SevROVPIDController::SevROVPIDController()
{

}
void SevROVPIDController::setKp(float value)
{
    Kp = value;
}
void SevROVPIDController::setKi(float value)
{
    Ki = value;
}
void SevROVPIDController::setKd(float value)
{
    Kd = value;
}
float SevROVPIDController::getKp()
{
    return Kp;
}
float SevROVPIDController::getKi()
{
    return Ki;
}
float SevROVPIDController::getKd()
{
    return Kd;
}
