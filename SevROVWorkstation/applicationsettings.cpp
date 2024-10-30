#include "applicationsettings.h"

ApplicationSettings::ApplicationSettings() {}

QString ApplicationSettings::getAppVersion()
{
    return QString(APP_VERSION.c_str());
}

void ApplicationSettings::load()
{
    _settingsFileName = QApplication::applicationDirPath() + QDir::separator() + "settings.ini";
    QSettings settings(_settingsFileName, QSettings::IniFormat);

    // Camera Settings
    settings.beginGroup("/CAMERA_SETTINGS");
    CAMERA_WIDTH = settings.value("/Width", "640").toInt();
    CAMERA_HEIGHT = settings.value("/Height", "480").toInt();
    CAMERA_FPS = settings.value("/FPS", "30").toInt();

    CAMERA_LEFT_ID = settings.value("/LeftID", "1").toInt();
    CAMERA_RIGHT_ID = settings.value("/RightID", "2").toInt();

    VIDEO_TIMER_INTERVAL = settings.value("/TimerInterval", "50").toInt();
    CAMERA_TYPE = settings.value("/CameraTypeID", 0).toInt();
    settings.endGroup();

    settings.beginGroup("/JOYSTICK");
    JOYSTICK_ID = settings.value("/JoystickID", "0").toInt();
    JOYSTICK_TIMER_INTERVAL = settings.value("/TimerInterval", "100").toInt();
    settings.endGroup();

    settings.beginGroup("/ROV");
    ROV_IP = settings.value("/IP", "127.0.0.1").toString();
    ROV_PORT = settings.value("/Port", 1234).toInt();
    settings.endGroup();
}

void ApplicationSettings::load(ControllerSettings &ctrset)
{
    this->load();

    _settingsFileName = QApplication::applicationDirPath() + QDir::separator() + "settings.ini";
    QSettings settings(_settingsFileName, QSettings::IniFormat);    

    // Controller Settigns
    settings.beginGroup("/CONTROLLER");
    ctrset.rollPID.setKp(settings.value("/rollKp", 0.1).toDouble());
    ctrset.rollPID.setKi(settings.value("/rollKi", 0.1).toDouble());
    ctrset.rollPID.setKd(settings.value("/rollKd", 0.1).toDouble());
    ctrset.rollStabilization = settings.value("/rollStabilization", false).toBool();

    ctrset.pitchPID.setKp(settings.value("/pitchKp", 0.1).toDouble());
    ctrset.pitchPID.setKi(settings.value("/pitchKi", 0.1).toDouble());
    ctrset.pitchPID.setKd(settings.value("/pitchKd", 0.1).toDouble());
    ctrset.pitchStabilization = settings.value("/pitchStabilization", false).toBool();

    ctrset.yawPID.setKp(settings.value("/yawKp", 0.1).toDouble());
    ctrset.yawPID.setKi(settings.value("/yawKi", 0.1).toDouble());
    ctrset.yawPID.setKd(settings.value("/yawKd", 0.1).toDouble());
    ctrset.yawStabilization = settings.value("/yawStabilization", false).toBool();

    ctrset.depthPID.setKp(settings.value("/depthKp", 0.1).toDouble());
    ctrset.depthPID.setKi(settings.value("/depthKi", 0.1).toDouble());
    ctrset.depthPID.setKd(settings.value("/depthKd", 0.1).toDouble());
    ctrset.depthStabilization = settings.value("/depthStabilization", false).toBool();

    ctrset.powerLimit = settings.value("/powerLimit", 0.1).toDouble();
    settings.endGroup();
}
