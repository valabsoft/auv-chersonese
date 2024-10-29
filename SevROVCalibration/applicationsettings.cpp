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
    settings.beginGroup("/CAMERA SETTINGS");
    CAMERA_WIDTH = settings.value("/Width", "640").toInt();
    CAMERA_HEIGHT = settings.value("/Height", "480").toInt();
    CAMERA_FPS = settings.value("/FPS", "30").toInt();

    CAMERA_LEFT_ID = settings.value("/LeftID", "1").toInt();
    CAMERA_RIGHT_ID = settings.value("/RightID", "2").toInt();

    VIDEO_TIMER_INTERVAL = settings.value("/TimerInterval", "50").toInt();
    CAMERA_TYPE = settings.value("/CameraTypeID", 0).toInt();
    settings.endGroup();
}
