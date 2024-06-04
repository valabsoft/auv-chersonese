#ifndef SEVROVPIDCONTROLLER_H
#define SEVROVPIDCONTROLLER_H


class SevROVPIDController
{
public:
    SevROVPIDController();
    void setKp(float value);
    void setKi(float value);
    void setKd(float value);
    float getKp();
    float getKi();
    float getKd();
private:
    float Kp;
    float Ki;
    float Kd;
};

#endif // SEVROVPIDCONTROLLER_H
