#include "sevrovlibrary.h"

void SevROVLibrary::XboxToControlData(const XboxGamepad xbox,
                                      const float powerlimit,
                                      const bool rollstabilization,
                                      const bool pitchstabilization,
                                      const bool yawstabilization,
                                      const bool depthstabilization,
                                      SevROVPIDController rollpid,
                                      SevROVPIDController pitchpid,
                                      SevROVPIDController yawpid,
                                      SevROVPIDController depthpid,
                                      bool updatepid,
                                      SevROVControlData *data)
{
    float leftX  = qFabs((float)xbox.LStickX) < JOYSTICK_DEAD_ZONE ? 0 : (float)xbox.LStickX / SHRT_MAX;
    float leftY  = qFabs((float)xbox.LStickY) < JOYSTICK_DEAD_ZONE ? 0 : -(float)xbox.LStickY / SHRT_MAX;
    data->setHorizontalVectorX(leftX);
    data->setHorizontalVectorY(leftY);

    float rightX  = qFabs((float)xbox.RStickY) < JOYSTICK_DEAD_ZONE ? 0 : -(float)xbox.RStickY / SHRT_MAX;
    float rightY  = qFabs((float)xbox.RStickX) < JOYSTICK_DEAD_ZONE ? 0 : (float)xbox.RStickX / SHRT_MAX;
    data->setAngularVelocityZ(rightY);
    data->setVericalThrust(rightX);

    auto gripDirection = xbox.LBumper - xbox.RBumper;
    data->setManipulatorState(gripDirection);

    float manipulatorRotateDirection = (float)(xbox.LTrigger - xbox.RTrigger) / SHRT_MAX / 2;
    data->setManipulatorRotate(manipulatorRotateDirection);

    // auto cameraRotateDirection = ((xbox.DPad == 1) ? 1 : 0) - ((xbox.DPad == 4) ? 1 : 0);
    auto cameraRotateDirection = xbox.Y - xbox.A;
    data->setCameraRotate(cameraRotateDirection);

    // xbox.Right - xbox.Left
    auto rollIncDir = ((xbox.DPad == 2) ? 1 : 0) - ((xbox.DPad == 8) ? 1 : 0);
    data->setRollInc(rollIncDir);

    // xbox.Up - xbox.Down
    auto pitchIncDir = ((xbox.DPad == 1) ? 1 : 0) - ((xbox.DPad == 4) ? 1 : 0);
    data->setPitchInc(pitchIncDir);

    auto reset = xbox.Menu;
    data->setResetInitialization(reset);

    if ((int8_t)xbox.X == 1) // Меняем состояние при нажатой кнопке Y
    {
        switch ((int8_t)data->getLightsState())
        {
        case 0:
            data->setLightsState((int8_t)1);
            break;
        case 1:
            data->setLightsState((int8_t)0);
            break;
        }
    }

    auto resetPos = xbox.B;
    data->setResetPosition(resetPos);

    data->setPowerTarget(powerlimit);

    data->setRollKp(rollpid.getKp());
    data->setRollKi(rollpid.getKi());
    data->setRollKd(rollpid.getKd());

    data->setPitchKp(pitchpid.getKp());
    data->setPitchKi(pitchpid.getKi());
    data->setPitchKd(pitchpid.getKd());

    data->setYawKp(yawpid.getKp());
    data->setYawKi(yawpid.getKi());
    data->setYawKd(yawpid.getKd());

    data->setDepthKp(depthpid.getKp());
    data->setDepthKi(depthpid.getKi());
    data->setDepthKd(depthpid.getKd());

    auto updatePID = updatepid ? 1 : 0;
    data->setUpdatePID(updatePID);

    int8_t stab_state = data->getStabilizationState();

    stab_state = (int8_t)(rollstabilization ? (stab_state | 0b00000001) : (stab_state & 0b11111110));
    stab_state = (int8_t)(pitchstabilization ? (stab_state | 0b00000010) : (stab_state & 0b11111101));
    stab_state = (int8_t)(yawstabilization ? (stab_state | 0b00000100) : (stab_state & 0b11111011));
    stab_state = (int8_t)(depthstabilization ? (stab_state | 0b00001000) : (stab_state & 0b11110111));

    data->setStabilizationState(stab_state);
}
